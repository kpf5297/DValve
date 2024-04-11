#include "PiStepper.h"
#include <iostream>

int ENABLE_PIN = 22;
int DIRECTION_PIN = 17;
int STEP_PIN = 27;
int STEPS_PER_REV = 200;
int MICROSTEPS = 1;

int main() {
    // Initialize the PiStepper object with GPIO pins for the step, direction, and enable lines
    // Adjust these pin numbers according to your hardware setup
    PiStepper motor(27, 17, 22, 200, 1);

    // Set motor speed and acceleration (optional, defaults may be used)
    motor.setSpeed(60); // Set motor speed to 60 RPM
    motor.setAcceleration(100); // Set motor acceleration to 100 RPM/s

    std::cout << "Homing motor..." << std::endl;
    motor.homeMotor(); // Home the motor to set its starting position

    std::cout << "Moving motor 400 steps forward..." << std::endl;
    motor.moveSteps(400, 1); // Move the motor 400 steps in one direction

    std::cout << "Current step count: " << motor.getCurrentStepCount() << std::endl;

    std::cout << "Moving motor 200 steps backward..." << std::endl;
    motor.moveSteps(200, 0); // Move the motor 200 steps in the opposite direction

    std::cout << "Current step count: " << motor.getCurrentStepCount() << std::endl;

    std::cout << "Rotating motor by 90 degrees..." << std::endl;
    motor.moveAngle(90, 1); // Rotate the motor by 90 degrees

    std::cout << "Current step count: " << motor.getCurrentStepCount() << std::endl;

    std::cout << "Homing motor again..." << std::endl;
    motor.homeMotor(); // Home the motor again

    std::cout << "Current step count after homing: " << motor.getCurrentStepCount() << std::endl;

    return 0;
}
