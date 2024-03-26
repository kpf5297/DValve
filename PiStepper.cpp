// PiStepper.cpp
#include "PiStepper.h"
#include <iostream>
#include <unistd.h>
#include <cmath>

/**
 * @brief Construct a new PiStepper object
 * 
 * @param stepPin The GPIO pin number for the step pin
 * @param dirPin The GPIO pin number for the direction pin
 * @param enablePin The GPIO pin number for the enable pin
 * @param stepsPerRevolution The number of steps per revolution for the stepper motor
 * @param microstepping The microstepping value for the stepper motor
 */
PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) {
    _stepPin = stepPin;
    _dirPin = dirPin;
    _enablePin = enablePin;
    _stepsPerRevolution = stepsPerRevolution;
    _microstepping = microstepping;
    _speed = 20;
    _acceleration = 80;

    // Initialize the GPIO pins
    chip = gpiod_chip_open("/dev/gpiochip0");
    step_signal = gpiod_chip_get_line(chip, _stepPin);
    dir_signal = gpiod_chip_get_line(chip, _dirPin);
    enable_signal = gpiod_chip_get_line(chip, _enablePin);

    // Configure the GPIO pins
    gpiod_line_request_output(step_signal, "PiStepper", 0);
    gpiod_line_request_output(dir_signal, "PiStepper", 0);
    gpiod_line_request_output(enable_signal, "PiStepper", 0);

    // Disable the stepper motor
    disable();
}


PiStepper::~PiStepper() {
    // Close the GPIO pins
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
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
    _remainingSteps = steps;
    _currentDirection = direction;
    
    // Set the direction
    gpiod_line_set_value(dir_signal, direction);

    // Define the starting speed, maximum speed, and acceleration
    double startSpeed = 0.1; // Adjust as needed
    double maxSpeed = 1.0; // Adjust as needed
    double acceleration = _acceleration; // Adjust as needed

    // Calculate the number of steps for acceleration and deceleration
    int accelSteps = (maxSpeed - startSpeed) / acceleration;
    int decelSteps = steps - accelSteps;

    // Enable the stepper motor
    enable();

    // Wait for the stepper motor to enable
    usleep(1000000);

    // Acceleration
    for (int i = 0; i < accelSteps; i++) {
        // Calculate the current speed
        double speed = startSpeed + i * acceleration;

        // Calculate the delay between steps
        double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(delayTime);
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Constant speed
    for (int i = accelSteps; i < decelSteps; i++) {
        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Deceleration
    for (int i = decelSteps; i < steps; i++) {
        // Calculate the current speed
        double speed = maxSpeed - (i - decelSteps) * acceleration;

        // Calculate the delay between steps
        double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(delayTime);
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }
}

void PiStepper::accelMoveSteps(int steps, int direction) {

    _remainingSteps = steps;
    _currentDirection = direction;
    

    // Set the direction
    gpiod_line_set_value(dir_signal, direction);

    // Calculate the delay between steps
    float delay = 1 / (_speed * _stepsPerRevolution * _microstepping);

    // Calculate the number of steps to accelerate
    int stepsToAccel = (_speed * _speed) / (2 * _acceleration * delay);

    // Calculate the number of steps to decelerate
    int stepsToDecel = stepsToAccel + stepsToAccel / 2;

    // Calculate the number of steps to run at constant speed
    int stepsAtSpeed = steps - stepsToDecel - stepsToAccel;

    // Enable the stepper motor
    enable();

    // Wait for the stepper motor to enable
    usleep(1000000);

    // Accelerate
    for (int i = 0; i < stepsToAccel; i++) {
        gpiod_line_set_value(step_signal, 1);
        usleep(delay * 1000000);
        gpiod_line_set_value(step_signal, 0);
        usleep(delay * 1000000);

        // Decrement the remaining steps
        _remainingSteps--;

    }

    // Run at constant speed
    for (int i = 0; i < stepsAtSpeed; i++) {
        gpiod_line_set_value(step_signal, 1);
        usleep(delay * 1000000);
        gpiod_line_set_value(step_signal, 0);
        usleep(delay * 1000000);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Decelerate
    for (int i = 0; i < stepsToDecel; i++) {
        gpiod_line_set_value(step_signal, 1);
        usleep(delay * 1000000);
        gpiod_line_set_value(step_signal, 0);
        usleep(delay * 1000000);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Disable the stepper motor
    disable();
}

void PiStepper::moveAngle(float angle, int direction) {

    _currentDirection = direction;
    

    int steps = round(angle * _stepsPerRevolution * _microstepping / 360);
    moveSteps(steps, direction);
}

void PiStepper::setMicrostepping(int microstepping) {
    

    _microstepping = microstepping;
}

void PiStepper::delay(float seconds) {
    

    usleep(seconds * 1000000);
}

bool PiStepper::move(int steps) {

    _remainingSteps = steps;

    // Define the starting speed, maximum speed, and acceleration
    double startSpeed = 0.1; // Adjust as needed
    double maxSpeed = 1.0; // Adjust as needed
    double acceleration = _acceleration; // Adjust as needed

    // Calculate the number of steps for acceleration and deceleration
    int accelSteps = (maxSpeed - startSpeed) / acceleration;
    int decelSteps = steps - accelSteps;

    // Enable the stepper motor
    enable();

    // Wait for the stepper motor to enable
    usleep(1000000);

    // Acceleration
    for (int i = 0; i < accelSteps; i++) {
        // Calculate the current speed
        double speed = startSpeed + i * acceleration;

        // Calculate the delay between steps
        double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(delayTime);
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Constant speed
    for (int i = accelSteps; i < decelSteps; i++) {
        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    // Deceleration
    for (int i = decelSteps; i < steps; i++) {
        // Calculate the current speed
        double speed = maxSpeed - (i - decelSteps) * acceleration;

        // Calculate the delay between steps
        double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

        // Move one step
        gpiod_line_set_value(step_signal, 0);
        usleep(delayTime);
        gpiod_line_set_value(step_signal, 1);

        // Decrement the remaining steps
        _remainingSteps--;
    }

    return true;
}

int PiStepper::getSteps() {
    return _remainingSteps;
}

int PiStepper::getDirection() {
    return _currentDirection;
}

void PiStepper::setDirection(int direction) {
    _currentDirection = direction;
}

void PiStepper::runMotor() {
    // Move the stepper motor
    moveSteps(getSteps(), getDirection());
}

void PiStepper::setSteps(int steps) {
    _remainingSteps = steps;
}

