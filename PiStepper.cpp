#include "PiStepper.h"
#include <cmath>
#include <unistd.h>

PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) :
    _stepPin(stepPin),
    _dirPin(dirPin),
    _enablePin(enablePin),
    _stepsPerRevolution(stepsPerRevolution),
    _microstepping(microstepping),
    _speed(20), // Default speed in RPM
    _acceleration(80), // Default acceleration in RPM/s
    _currentStepCount(0) // Initialize step counter to 0
{
    chip = gpiod_chip_open("/dev/gpiochip0");
    step_signal = gpiod_chip_get_line(chip, _stepPin);
    dir_signal = gpiod_chip_get_line(chip, _dirPin);
    enable_signal = gpiod_chip_get_line(chip, _enablePin);
    // limit_switch_top = gpiod_chip_get_line(chip, LIMIT_SWITCH_TOP_PIN);
    limit_switch_bottom = gpiod_chip_get_line(chip, LIMIT_SWITCH_BOTTOM_PIN);

    // Configure GPIO pins
    gpiod_line_request_output(step_signal, "PiStepper_step", 0);
    gpiod_line_request_output(dir_signal, "PiStepper_dir", 0);
    gpiod_line_request_output(enable_signal, "PiStepper_enable", 1);
    gpiod_line_request_input(limit_switch_bottom, "PiStepper_limit_bottom");
    // gpiod_line_request_input(limit_switch_top, "PiStepper_limit_top");

    disable(); // Start with the motor disabled
}

PiStepper::~PiStepper() {
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
    // gpiod_line_release(limit_switch_top);
    gpiod_line_release(limit_switch_bottom);
    gpiod_chip_close(chip);
}

void PiStepper::setSpeed(float speed) {
    _speed = speed;
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
    std::lock_guard<std::mutex> lock(gpioMutex); // Ensure thread-safe access to GPIO
    enable();
    gpiod_line_set_value(dir_signal, direction);

    for (int i = 0; i < steps; i++) {
        // Check if a limit switch is activated, stop if it is
        // if (gpiod_line_get_value(limit_switch_top) == 1 || gpiod_line_get_value(limit_switch_bottom) == 1) {
        //     break;
        // }
        if (gpiod_line_get_value(limit_switch_bottom) == 1 && direction == 0) {
            break;
        }

        // Move one step
        gpiod_line_set_value(step_signal, 1);
        usleep(1000); // This delay may need to be adjusted
        gpiod_line_set_value(step_signal, 0);
        usleep(1000); // This delay may need to be adjusted

        // Update the current step count
        _currentStepCount += (direction == 0) ? -1 : 1;
    }
}

void PiStepper::moveAngle(float angle, int direction) {
    int steps = std::round(angle / 360.0 * _stepsPerRevolution * _microstepping);
    moveSteps(steps, direction);
}

void PiStepper::homeMotor() {
    enable();
    const int direction = 0; // For closing the valve
    while (gpiod_line_get_value(limit_switch_bottom) == 0) { // Assumes active high when triggered
        moveSteps(1, direction); // Move one step at a time towards the home position
        // adding a delay 
        usleep(10000); // Sleep for 10 ms
        
    }
    _currentStepCount = 0; // Reset the step counter at the home position
    disable();
}


void PiStepper::setMicrostepping(int microstepping) {
    _microstepping = microstepping;
}

int PiStepper::getCurrentStepCount() const {
    return _currentStepCount;
}

float PiStepper::stepsToAngle(int steps) {
    return (static_cast<float>(steps) / (_stepsPerRevolution * _microstepping)) * 360.0f;
}
