#include "PiStepper.h"
#include <cmath>
#include <unistd.h>
#include <thread>

PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) :
    _stepPin(stepPin),
    _dirPin(dirPin),
    _enablePin(enablePin),
    _stepsPerRevolution(stepsPerRevolution),
    _microstepping(microstepping),
    _speed(DEFAULT_SPEED), // Default speed in RPM
    _acceleration(DEFAULT_ACCELERATION), // Default acceleration in RPM/s
    _currentStepCount(0), // Initialize step counter to 0
    _fullRangeCount(0), // Initialize full range count to 0
    _isMoving(false), // Initialize moving flag to false
    _isCalibrated(false) // Initialize calibrated flag to false
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    step_signal = gpiod_chip_get_line(chip, _stepPin);
    dir_signal = gpiod_chip_get_line(chip, _dirPin);
    enable_signal = gpiod_chip_get_line(chip, _enablePin);
    limit_switch_top = gpiod_chip_get_line(chip, LIMIT_SWITCH_TOP_PIN);
    limit_switch_bottom = gpiod_chip_get_line(chip, LIMIT_SWITCH_BOTTOM_PIN);

    // Configure GPIO pins
    gpiod_line_request_output(step_signal, "PiStepper_step", 0);
    gpiod_line_request_output(dir_signal, "PiStepper_dir", 0);
    gpiod_line_request_output(enable_signal, "PiStepper_enable", 1);
    gpiod_line_request_input(limit_switch_bottom, "PiStepper_limit_bottom");
    gpiod_line_request_input(limit_switch_top, "PiStepper_limit_top");

    disable(); // Start with the motor disabled
}

PiStepper::PiStepper() :
    PiStepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING) {};

PiStepper::~PiStepper() {
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
    gpiod_line_release(limit_switch_top);
    gpiod_line_release(limit_switch_bottom);
    gpiod_chip_close(chip);
}

void PiStepper::setSpeed(float speed) {
    if (speed <= MAX_SPEED) {
        _speed = speed;
    }
}

void PiStepper::setAcceleration(float acceleration) {
    _acceleration = acceleration;
}

void PiStepper::enable() {
    gpiod_line_set_value(enable_signal, 1); 
}

void PiStepper::disable() {
    gpiod_line_set_value(enable_signal, 0); 
}

void PiStepper::moveSteps(int steps, int direction) {
    {
        std::lock_guard<std::mutex> lock(gpioMutex);
        if (!_isCalibrated) {
            std::cerr << "Calibration is required before moving the motor." << std::endl;
            return;
        }
        _isMoving = true;
    }
    
    enable();
    gpiod_line_set_value(dir_signal, direction);

    float stepDelay = 60.0 * 1000000 / (_speed * _stepsPerRevolution * _microstepping); // delay in microseconds

    for (int i = 0; i < steps; i++) {
        {
            std::lock_guard<std::mutex> lock(gpioMutex);
            if (!_isMoving) {
                std::cout << "Movement stopped by user." << std::endl;
                break;
            }
        }

        if (gpiod_line_get_value(limit_switch_top) == 0 && direction == 1) {
            std::cout << "Top limit switch triggered" << std::endl;
            break;
        }

        if (gpiod_line_get_value(limit_switch_bottom) == 0 && direction == 0) {
            std::cout << "Bottom limit switch triggered" << std::endl;
            break;
        }

        gpiod_line_set_value(step_signal, 1);
        usleep(stepDelay / 2); // Half delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(stepDelay / 2); // Half delay for pulse low

        {
            std::lock_guard<std::mutex> lock(gpioMutex);
            if (direction == 0) {
                _currentStepCount--;
            } else {
                _currentStepCount++;
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(gpioMutex);
        _isMoving = false;
    }
    disable();
}

void PiStepper::moveAngle(float angle, int direction) {
    int steps = std::round(angle * ((_stepsPerRevolution * _microstepping) / 360.0f));
    moveSteps(steps, direction);
}

void PiStepper::moveStepsAsync(int steps, int direction, std::function<void()> callback) {
    std::thread([this, steps, direction, callback]() {
        moveSteps(steps, direction);
        if (callback) {
            callback();
        }
    }).detach();
}

void PiStepper::stopMovement() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
}

void PiStepper::emergencyStop() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
    disable();
    _currentStepCount = 0; // Optionally reset step count
    std::cout << "Emergency Stop Activated!" << std::endl;
}

void PiStepper::calibrate() {
    enable();
    _currentStepCount = 0; // Reset step count
    _fullRangeCount = 0; // Reset full range count

    // Move to bottom limit switch
    gpiod_line_set_value(dir_signal, 0);
    while (gpiod_line_get_value(limit_switch_bottom) == 1) {
        gpiod_line_set_value(step_signal, 1);
        usleep(4000); // Short delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(4000); // Short delay for pulse low
    }

    // Move to top limit switch
    gpiod_line_set_value(dir_signal, 1);
    while (gpiod_line_get_value(limit_switch_top) == 1) {
        gpiod_line_set_value(step_signal, 1);
        usleep(2000); // Short delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(2000); // Short delay for pulse low
        _fullRangeCount++;
    }

    _currentStepCount = _fullRangeCount; // Set current step count to full range
    _isCalibrated = true; // Set calibrated flag to true
    disable();
    std::cout << "Calibration complete. Full range: " << _fullRangeCount << " steps." << std::endl;
}

void PiStepper::setMicrostepping(int microstepping) {
    _microstepping = microstepping;
}

int PiStepper::getCurrentStepCount() const {
    std::lock_guard<std::mutex> lock(gpioMutex);
    return _currentStepCount;
}

int PiStepper::getFullRangeCount() const {
    std::lock_guard<std::mutex> lock(gpioMutex);
    return _fullRangeCount;
}

float PiStepper::getPercentOpen() const {
    std::lock_guard<std::mutex> lock(gpioMutex);
    return (_currentStepCount / static_cast<float>(_fullRangeCount)) * 100.0f;
}

bool PiStepper::isMoving() const {
    std::lock_guard<std::mutex> lock(gpioMutex);
    return _isMoving;
}

void PiStepper::moveToPercentOpen(float percent, std::function<void()> callback) {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    int targetStepCount = static_cast<int>((percent / 100.0f) * _fullRangeCount);
    int stepsToMove = targetStepCount - _currentStepCount;
    int direction = (stepsToMove >= 0) ? 1 : 0;
    moveStepsAsync(abs(stepsToMove), direction, callback);
}

void PiStepper::moveToFullyOpen() {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    moveStepsAsync(_fullRangeCount - _currentStepCount, 1, [this]() {
        std::cout << "Motor moved to fully open position." << std::endl;
    });
    
    // moveSteps(_fullRangeCount - _currentStepCount, 1);
}

void PiStepper::moveToFullyClosed() {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    moveStepsAsync(_currentStepCount, 0, [this]() {
        std::cout << "Motor moved to fully closed position." << std::endl;
    });

    // moveSteps(_currentStepCount, 0);
}

int PiStepper::getStepsPerRevolution() const {
    return _stepsPerRevolution;
}

int PiStepper::getMicrostepping() const {
    return _microstepping;
}

float PiStepper::getSpeed() const {
    return _speed;
}

float PiStepper::getAcceleration() const {
    return _acceleration;
}
