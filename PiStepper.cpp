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
    limit_switch_bottom = gpiod_chip_get_line(chip, LIMIT_SWITCH_BOTTOM_PIN);

    // Configure GPIO pins
    gpiod_line_request_output(step_signal, "PiStepper_step", 0);
    gpiod_line_request_output(dir_signal, "PiStepper_dir", 0);
    gpiod_line_request_output(enable_signal, "PiStepper_enable", 1);
    gpiod_line_request_input(limit_switch_bottom, "PiStepper_limit_bottom");

    disable(); // Start with the motor disabled
}

PiStepper::~PiStepper() {
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
    gpiod_line_release(limit_switch_top);
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
        if (gpiod_line_get_value(limit_switch_top) == 1 || gpiod_line_get_value(limit_switch_bottom) == 1) {
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


// // PiStepper.cpp
// #include "PiStepper.h"
// #include <iostream>
// #include <unistd.h>
// #include <cmath>

// /**
//  * @brief Construct a new PiStepper object
//  * 
//  * @param stepPin The GPIO pin number for the step pin
//  * @param dirPin The GPIO pin number for the direction pin
//  * @param enablePin The GPIO pin number for the enable pin
//  * @param stepsPerRevolution The number of steps per revolution for the stepper motor
//  * @param microstepping The microstepping value for the stepper motor
//  */
// PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) {
//     _stepPin = stepPin;
//     _dirPin = dirPin;
//     _enablePin = enablePin;
//     _stepsPerRevolution = stepsPerRevolution;
//     _microstepping = microstepping;
//     _speed = 20;
//     _acceleration = 80;

//     // Initialize the GPIO pins
//     chip = gpiod_chip_open("/dev/gpiochip0");
//     step_signal = gpiod_chip_get_line(chip, _stepPin);
//     dir_signal = gpiod_chip_get_line(chip, _dirPin);
//     enable_signal = gpiod_chip_get_line(chip, _enablePin);

//     // Configure the GPIO pins
//     gpiod_line_request_output(step_signal, "PiStepper", 0);
//     gpiod_line_request_output(dir_signal, "PiStepper", 0);
//     gpiod_line_request_output(enable_signal, "PiStepper", 0);

//     // Disable the stepper motor
//     disable();
// }


// PiStepper::~PiStepper() {
//     // Close the GPIO pins
//     gpiod_line_release(step_signal);
//     gpiod_line_release(dir_signal);
//     gpiod_line_release(enable_signal);
//     gpiod_chip_close(chip);
// }


// void PiStepper::setSpeed(float speed) {
//     _speed = speed;
// }


// void PiStepper::setAcceleration(float acceleration) {
//     _acceleration = acceleration;
// }


// void PiStepper::enable() {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     gpiod_line_set_value(enable_signal, 1);
// }


// void PiStepper::disable() {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     gpiod_line_set_value(enable_signal, 0);
// }

// void PiStepper::moveSteps(int steps, int direction) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     _remainingSteps = steps;
//     _currentDirection = direction;
    
//     // Set the direction
//     gpiod_line_set_value(dir_signal, direction);

//     // Define the starting speed, maximum speed, and acceleration
//     double startSpeed = 0.1; // Adjust as needed
//     double maxSpeed = 1.2; // Adjust as needed
//     double acceleration = _acceleration; // Adjust as needed

//     // Calculate the number of steps for acceleration and deceleration
//     int accelSteps = (maxSpeed - startSpeed) / acceleration;
//     int decelSteps = steps - accelSteps;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Acceleration
//     for (int i = 0; i < accelSteps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Calculate the current speed
//         double speed = startSpeed + i * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Constant speed
//     for (int i = accelSteps; i < decelSteps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Deceleration
//     for (int i = decelSteps; i < steps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Calculate the current speed
//         double speed = maxSpeed - (i - decelSteps) * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }
// }

// void PiStepper::accelMoveSteps(int steps, int direction) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block

//     _remainingSteps = steps;
//     _currentDirection = direction;
    

//     // Set the direction
//     gpiod_line_set_value(dir_signal, direction);

//     // Calculate the delay between steps
//     float delay = 1 / (_speed * _stepsPerRevolution * _microstepping);

//     // Calculate the number of steps to accelerate
//     int stepsToAccel = (_speed * _speed) / (2 * _acceleration * delay);

//     // Calculate the number of steps to decelerate
//     int stepsToDecel = stepsToAccel + stepsToAccel / 2;

//     // Calculate the number of steps to run at constant speed
//     int stepsAtSpeed = steps - stepsToDecel - stepsToAccel;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Accelerate
//     for (int i = 0; i < stepsToAccel; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;

//     }

//     // Run at constant speed
//     for (int i = 0; i < stepsAtSpeed; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Decelerate
//     for (int i = 0; i < stepsToDecel; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Disable the stepper motor
//     disable();
// }

// void PiStepper::moveAngle(float angle, int direction) {

//     _currentDirection = direction;
    

//     int steps = round(angle * _stepsPerRevolution * _microstepping / 360);
//     moveSteps(steps, direction);
// }

// void PiStepper::setMicrostepping(int microstepping) {
    

//     _microstepping = microstepping;
// }

// void PiStepper::delay(float seconds) {
    

//     usleep(seconds * 1000000);
// }

// bool PiStepper::move(int steps) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block

//     _remainingSteps = steps;

//     // Define the starting speed, maximum speed, and acceleration
//     double startSpeed = 0.1; // Adjust as needed
//     double maxSpeed = 1.0; // Adjust as needed
//     double acceleration = _acceleration; // Adjust as needed

//     // Calculate the number of steps for acceleration and deceleration
//     int accelSteps = (maxSpeed - startSpeed) / acceleration;
//     int decelSteps = steps - accelSteps;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Acceleration
//     for (int i = 0; i < accelSteps; i++) {
//         // Calculate the current speed
//         double speed = startSpeed + i * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Constant speed
//     for (int i = accelSteps; i < decelSteps; i++) {
//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Deceleration
//     for (int i = decelSteps; i < steps; i++) {
//         // Calculate the current speed
//         double speed = maxSpeed - (i - decelSteps) * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     return true;
// }

// int PiStepper::getSteps() {
//     return _remainingSteps;
// }

// int PiStepper::getDirection() {
//     return _currentDirection;
// }

// void PiStepper::setDirection(int direction) {
//     _currentDirection = direction;
// }

// void PiStepper::runMotor() {
//     // Move the stepper motor
//     moveSteps(getSteps(), getDirection());
// }

// void PiStepper::setSteps(int steps) {
//     _remainingSteps = steps;
// }

