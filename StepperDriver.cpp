/*
    To compile: g++ -o StepperDriver StepperDriver.cpp PiStepper.cpp -lgpiod -pthread


*/

#include "PiStepper.h"
#include <iostream>

int ENABLE_PIN = 22;
int DIRECTION_PIN = 17;
int STEP_PIN = 27;
int STEPS_PER_REV = 200;
int MICROSTEPS = 1;

/*
    Test: Timed test for RPM input
    Description: The following code snippet allows the user to input an RPM value to move the motor.  The motor will move 200 steps (1 full rotation) 
    at the specified speed.  The time taken to move the motor will be displayed at the end of each movement.  The user can input q to quit the 
    program or h to send the motor to the home position.
*/
void rpmTest(PiStepper& motor) {
    // Declare variables to measure time taken to move the motor
    std::chrono::steady_clock::time_point start, end;
    std::chrono::duration<double> duration;

    std::string input;
    double rpm;
    bool quit = false;

    while (!quit) {
        std::cout << "Enter RPM (q to quit, h to home): ";
        std::cin >> input;

        if (input == "q") {
            quit = true;
        } else if (input == "h") {
            std::cout << "Homing motor..." << std::endl;
            motor.homeMotor();
        } else {
            rpm = std::stod(input);
            motor.setSpeed(rpm);
            std::cout << "Moving motor 200 steps forward..." << std::endl;
            start = std::chrono::steady_clock::now();
            motor.moveSteps(200, 1);
            end = std::chrono::steady_clock::now();
            duration = end - start;
            std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
        }
    }
}

/*
    Test: Duration test for moveStepsOverDuration method
    Description: The following code snippet allows the user to input the number of steps and the duration to move the motor. The motor will move the specified number of steps over the specified duration. The time taken to move will be displayed at the end of each movement. The user can input q to quit the program or h to send the motor to the home position.
*/
void durationTest(PiStepper& motor) {
    // Declare variables to measure time taken to move the motor
    std::chrono::steady_clock::time_point start, end;
    std::chrono::duration<double> duration;
    std::string input;
    int steps;
    double durationSec;
    bool quit = false;

    while (!quit) {
        std::cout << "Enter number of steps to open valve (q to quit, h to home): ";
        std::cin >> input;
        if (input == "q") {
            quit = true;
        } else if (input == "h") {
            std::cout << "Homing motor..." << std::endl;
            motor.homeMotor();
        } else {
            steps = std::stoi(input);
            std::cout << "Enter duration in seconds: ";
            std::cin >> durationSec;
            std::cout << "Moving motor " << steps << " steps over " << durationSec << " seconds..." << std::endl;
            start = std::chrono::steady_clock::now();
            motor.moveStepsOverDuration(steps, durationSec);
            end = std::chrono::steady_clock::now();
            duration = end - start;
            std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
        }
    }
}

int main() {
    // Initialize the PiStepper object with GPIO pins for the step, direction, and enable lines
    // Adjust these pin numbers according to your hardware setup
    PiStepper motor(STEP_PIN, DIRECTION_PIN, ENABLE_PIN, STEPS_PER_REV, MICROSTEPS);

    // rpmTest(motor);

    durationTest(motor);

    return 0;
}
