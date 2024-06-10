/*
    PiStepperDriver.cpp - Driver program for testing the PiStepper class.
    Compile with: g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp -lgpiod -pthread

    Usage: Run the program and follow the menu options to test various features of the PiStepper class.
*/

#include "PiStepper.h"
#include <iostream>
#include <functional>

// GPIO pins configuration
int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

// Instantiate PiStepper object
PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);

// Function prototypes
void printMenu();
void setSpeed();
void setAcceleration();
void moveSteps();
void moveAngle();
void homeMotor();
void calibrateMotor();
void getCurrentStepCount();
void getFullRangeCount();
void emergencyStop();
void moveToPercentOpen();
void getPercentOpen();

int main() {
    int choice;
    do {
        printMenu();
        std::cin >> choice;
        switch (choice) {
            case 1:
                setSpeed();
                break;
            case 2:
                setAcceleration();
                break;
            case 3:
                moveSteps();
                break;
            case 4:
                moveAngle();
                break;
            case 5:
                homeMotor();
                break;
            case 6:
                calibrateMotor();
                break;
            case 7:
                getCurrentStepCount();
                break;
            case 8:
                getFullRangeCount();
                break;
            case 9:
                emergencyStop();
                break;
            case 10:
                moveToPercentOpen();
                break;
            case 11:
                getPercentOpen();
                break;
            case 0:
                std::cout << "Exiting..." << std::endl;
                break;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
        }
    } while (choice != 0);
    return 0;
}

void printMenu() {
    std::cout << "\nPiStepper Driver Menu" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "1. Set Speed (RPM)" << std::endl;
    std::cout << "2. Set Acceleration (RPM/s)" << std::endl;
    std::cout << "3. Move Steps" << std::endl;
    std::cout << "4. Move Angle" << std::endl;
    std::cout << "5. Home Motor" << std::endl;
    std::cout << "6. Calibrate Motor" << std::endl;
    std::cout << "7. Get Current Step Count" << std::endl;
    std::cout << "8. Get Full Range Count" << std::endl;
    std::cout << "9. Emergency Stop" << std::endl;
    std::cout << "10. Move to Percent Open" << std::endl;
    std::cout << "11. Get Percent Open" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "Select an option: ";
}

void setSpeed() {
    float speed;
    std::cout << "Enter speed (RPM): ";
    std::cin >> speed;
    stepper.setSpeed(speed);
    std::cout << "Speed set to " << speed << " RPM." << std::endl;
}

void setAcceleration() {
    float acceleration;
    std::cout << "Enter acceleration (RPM/s): ";
    std::cin >> acceleration;
    stepper.setAcceleration(acceleration);
    std::cout << "Acceleration set to " << acceleration << " RPM/s." << std::endl;
}

void moveSteps() {
    int steps, direction;
    std::cout << "Enter number of steps: ";
    std::cin >> steps;
    std::cout << "Enter direction (0 = close, 1 = open): ";
    std::cin >> direction;
    stepper.moveSteps(steps, direction);
    std::cout << "Moved " << steps << " steps in direction " << direction << "." << std::endl;
}

void moveAngle() {
    float angle;
    int direction;
    std::cout << "Enter angle (degrees): ";
    std::cin >> angle;
    std::cout << "Enter direction (0 = close, 1 = open): ";
    std::cin >> direction;
    stepper.moveAngle(angle, direction);
}

void homeMotor() {
    stepper.homeMotor();
    std::cout << "Motor homed." << std::endl;
}

void calibrateMotor() {
    stepper.calibrate();
    std::cout << "Motor calibrated." << std::endl;
}

void getCurrentStepCount() {
    int count = stepper.getCurrentStepCount();
    std::cout << "Current step count: " << count << std::endl;
}

void getFullRangeCount() {
    int range = stepper.getFullRangeCount();
    std::cout << "Full range count: " << range << " steps." << std::endl;
}

void emergencyStop() {
    stepper.emergencyStop();
    std::cout << "Emergency Stop Activated!" << std::endl;
}

void moveToPercentOpen() {
    float percent;
    std::cout << "Enter percent open (0-100): ";
    std::cin >> percent;
    stepper.moveToPercentOpen(percent, [](){
        std::cout << "Move to percent open complete." << std::endl;
    });
}

void getPercentOpen() {
    float percent = stepper.getPercentOpen();
    std::cout << "Current position: " << percent << "% open." << std::endl;
}
