#include "PiStepper.h"
#include <cmath>
#include <unistd.h>

// Define limit switch pin numbers
#define LIMIT_SWITCH_TOP_PIN 20
#define LIMIT_SWITCH_BOTTOM_PIN 21

/**
 * @brief Constructor for PiStepper.
 * @param stepPin The GPIO pin number for the step signal.
 * @param dirPin The GPIO pin number for the direction signal.
 * @param enablePin The GPIO pin number for the enable signal.
 * @param stepsPerRevolution The number of steps per full revolution of the motor.
 * @param microstepping The microstepping value for the motor.
 */
PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) :
    _stepPin(stepPin),
    _dirPin(dirPin),
    _enablePin(enablePin),
    _stepsPerRevolution(stepsPerRevolution),
    _microstepping(microstepping),
    _speed(20), // Default speed in RPM
    _maxSpeed(60), // Default max speed in RPM
    _acceleration(80), // Default acceleration in RPM/s
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

/**
 * @brief Destructor for PiStepper.
 */
PiStepper::~PiStepper() {
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
    gpiod_line_release(limit_switch_top);
    gpiod_line_release(limit_switch_bottom);
    gpiod_chip_close(chip);
}

/**
 * @brief Sets the speed of the stepper motor.
 * @param speed The speed in RPM.
 */
void PiStepper::setSpeed(float speed) {
    _speed = speed;
}

/**
 * @brief Sets the maximum speed of the stepper motor.
 * @param maxSpeed The maximum speed in RPM.
 */
void PiStepper::setMaxSpeed(float maxSpeed) {
    _maxSpeed = maxSpeed;
}

/**
 * @brief Sets the acceleration of the stepper motor.
 * @param acceleration The acceleration in RPM/s.
 */
void PiStepper::setAcceleration(float acceleration) {
    _acceleration = acceleration;
}

/**
 * @brief Enables the stepper motor.
 */
void PiStepper::enable() {
    gpiod_line_set_value(enable_signal, 1); 
}

/**
 * @brief Disables the stepper motor.
 */
void PiStepper::disable() {
    gpiod_line_set_value(enable_signal, 0); 
}

/**
 * @brief Moves the stepper motor a specified number of steps.
 * @param steps The number of steps to move.
 * @param direction The direction to move (0 for closed, 1 for open).
 */
void PiStepper::moveSteps(int steps, int direction) {
    std::lock_guard<std::mutex> lock(gpioMutex);
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    _isMoving = true;
    enable();
    gpiod_line_set_value(dir_signal, direction);

    float stepDelay = 60.0 * 1000000 / (_speed * _stepsPerRevolution * _microstepping); // delay in microseconds

    for (int i = 0; i < steps && _isMoving; i++) {
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

        if (direction == 0) {
            _currentStepCount--;
        } else {
            _currentStepCount++;
        }
    }
    _isMoving = false;
    disable();
}

/**
 * @brief Moves the stepper motor a specified angle.
 * @param angle The angle to move in degrees.
 * @param direction The direction to move (0 for closed, 1 for open).
 */
void PiStepper::moveAngle(float angle, int direction) {
    int steps = std::round(angle * ((_stepsPerRevolution * _microstepping) / 360.0f));
    moveSteps(steps, direction);
}

/**
 * @brief Moves the stepper motor asynchronously a specified number of steps.
 * @param steps The number of steps to move.
 * @param direction The direction to move (0 for closed, 1 for open).
 * @param callback A callback function to call when the movement is complete.
 */
void PiStepper::moveStepsAsync(int steps, int direction, std::function<void()> callback) {
    std::thread([this, steps, direction, callback]() {
        moveSteps(steps, direction);
        if (callback) {
            callback();
        }
    }).detach();
}

/**
 * @brief Stops the movement of the stepper motor.
 */
void PiStepper::stopMovement() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
    disable();
}

/**
 * @brief Performs an emergency stop of the stepper motor.
 */
void PiStepper::emergencyStop() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
    disable();
    _currentStepCount = 0; // Optionally reset step count
    std::cout << "Emergency Stop Activated!" << std::endl;
}

/**
 * @brief Calibrates the stepper motor using the limit switches.
 */
void PiStepper::calibrate() {
    enable();
    _currentStepCount = 0; // Reset step count
    _fullRangeCount = 0; // Reset full range count

    // Move to bottom limit switch
    gpiod_line_set_value(dir_signal, 0);
    while (gpiod_line_get_value(limit_switch_bottom) == 1) {
        gpiod_line_set_value(step_signal, 1);
        usleep(2000); // Short delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(2000); // Short delay for pulse low
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

/**
 * @brief Moves the stepper motor to a specified percent open position.
 * @param percent The percent open position (0-100).
 * @param callback A callback function to call when the movement is complete.
 */
void PiStepper::moveToPercentOpen(float percent, std::function<void()> callback) {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    if (percent < 0 || percent > 100) {
        std::cerr << "Percent open must be between 0 and 100." << std::endl;
        return;
    }
    int targetSteps = static_cast<int>((percent / 100.0) * _fullRangeCount);
    int stepsToMove = targetSteps - _currentStepCount;
    int direction = (stepsToMove >= 0) ? 1 : 0;
    moveStepsAsync(std::abs(stepsToMove), direction, callback);
}

/**
 * @brief Moves the stepper motor to the fully open position.
 */
void PiStepper::moveToFullyOpen() {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    moveStepsAsync(_fullRangeCount - _currentStepCount, 1, []() {
        std::cout << "Move to fully open completed." << std::endl;
    });
}

/**
 * @brief Moves the stepper motor to the fully closed position.
 */
void PiStepper::moveToFullyClosed() {
    if (!_isCalibrated) {
        std::cerr << "Calibration is required before moving the motor." << std::endl;
        return;
    }
    moveStepsAsync(_currentStepCount, 0, []() {
        std::cout << "Move to fully closed completed." << std::endl;
    });
}

/**
 * @brief Gets the current step count of the stepper motor.
 * @return The current step count.
 */
int PiStepper::getCurrentStepCount() const {
    return _currentStepCount;
}

/**
 * @brief Gets the full range step count of the stepper motor.
 * @return The full range step count.
 */
int PiStepper::getFullRangeCount() const {
    return _fullRangeCount;
}

/**
 * @brief Gets the percent open position of the stepper motor.
 * @return The percent open position.
 */
float PiStepper::getPercentOpen() const {
    if (!_isCalibrated) {
        return 0.0;
    }
    return (static_cast<float>(_currentStepCount) / _fullRangeCount) * 100.0;
}

/**
 * @brief Checks if the stepper motor is currently moving.
 * @return True if the motor is moving, false otherwise.
 */
bool PiStepper::isMoving() const {
    return _isMoving;
}

/**
 * @brief Converts steps to angle in degrees.
 * @param steps The number of steps.
 * @return The angle in degrees.
 */
float PiStepper::stepsToAngle(int steps) {
    return (static_cast<float>(steps) / (_stepsPerRevolution * _microstepping)) * 360.0f;
}
