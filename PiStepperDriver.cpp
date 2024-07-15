/**
 * @file PiStepperDriver.cpp
 * @brief Driver code to test the PiStepper class
 * 
 * Compilation:
 * g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp -lgpiod -pthread
 */

#include <iostream>
#include <thread>
#include <chrono>
#include "PiStepper.h"

// Function prototypes
void displayMenu();
void handleMoveSteps(PiStepper& stepper);
void handleMoveAngle(PiStepper& stepper);
void handleCalibrate(PiStepper& stepper);
void handleMoveToPercentOpen(PiStepper& stepper);
void handleMoveToFullyOpen(PiStepper& stepper);
void handleMoveToFullyClosed(PiStepper& stepper);
void handleEmergencyStop(PiStepper& stepper);
void handleGetStatus(PiStepper& stepper);

int main() {
    int stepPin = 27;
    int dirPin = 17;
    int enablePin = 22;
    PiStepper stepper(stepPin, dirPin, enablePin, 200, 1); // Microstepping set to 1

    char choice;
    do {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case '1':
                handleMoveSteps(stepper);
                break;
            case '2':
                handleMoveAngle(stepper);
                break;
            case '3':
                handleCalibrate(stepper);
                break;
            case '4':
                handleMoveToPercentOpen(stepper);
                break;
            case '5':
                handleMoveToFullyOpen(stepper);
                break;
            case '6':
                handleMoveToFullyClosed(stepper);
                break;
            case '7':
                handleEmergencyStop(stepper);
                break;
            case '8':
                handleGetStatus(stepper);
                break;
            case 'q':
                std::cout << "Quitting..." << std::endl;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    } while (choice != 'q');

    return 0;
}

void displayMenu() {
    std::cout << "Menu:\n";
    std::cout << "1. Move Steps\n";
    std::cout << "2. Move Angle\n";
    std::cout << "3. Calibrate\n";
    std::cout << "4. Move to Percent Open\n";
    std::cout << "5. Move to Fully Open\n";
    std::cout << "6. Move to Fully Closed\n";
    std::cout << "7. Emergency Stop\n";
    std::cout << "8. Get Status\n";
    std::cout << "q. Quit\n";
    std::cout << "Enter your choice: ";
}

void handleMoveSteps(PiStepper& stepper) {
    int steps, direction;
    std::cout << "Enter steps: ";
    std::cin >> steps;
    std::cout << "Enter direction (0 for closing, 1 for opening): ";
    std::cin >> direction;
    stepper.moveStepsAsync(steps, direction, []() {
        std::cout << "Move Steps operation completed." << std::endl;
    });
}

void handleMoveAngle(PiStepper& stepper) {
    float angle;
    int direction;
    std::cout << "Enter angle (degrees): ";
    std::cin >> angle;
    std::cout << "Enter direction (0 for closing, 1 for opening): ";
    std::cin >> direction;
    stepper.moveAngle(angle, direction);
    std::cout << "Move Angle operation completed." << std::endl;
}

void handleCalibrate(PiStepper& stepper) {
    stepper.calibrate();
}

void handleMoveToPercentOpen(PiStepper& stepper) {
    float percent;
    std::cout << "Enter percent open (0-100): ";
    std::cin >> percent;
    stepper.moveToPercentOpen(percent, []() {
        std::cout << "Move to Percent Open operation completed." << std::endl;
    });
}

void handleMoveToFullyOpen(PiStepper& stepper) {
    stepper.moveToFullyOpen();
}

void handleMoveToFullyClosed(PiStepper& stepper) {
    stepper.moveToFullyClosed();
}

void handleEmergencyStop(PiStepper& stepper) {
    stepper.emergencyStop();
}

void handleGetStatus(PiStepper& stepper) {
    std::cout << "Current Step Count: " << stepper.getCurrentStepCount() << std::endl;
    std::cout << "Full Range Count: " << stepper.getFullRangeCount() << std::endl;
    std::cout << "Percent Open: " << stepper.getPercentOpen() << "%" << std::endl;
    std::cout << "Moving: " << (stepper.isMoving() ? "Yes" : "No") << std::endl;
}
