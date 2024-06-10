/**
 * @file PiStepperDriver.cpp
 * @brief Driver for the PiStepper class, demonstrating its usage.
 * 
 * Compile with: g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp -lgpiod -pthread
 */

#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include "PiStepper.h"

/**
 * @brief Handles moving the stepper motor by steps.
 * @param stepper Reference to the PiStepper object.
 */
void handleMoveSteps(PiStepper &stepper) {
    int steps, direction;
    std::cout << "Enter steps to move: ";
    std::cin >> steps;
    std::cout << "Enter direction (0 for closed, 1 for open): ";
    std::cin >> direction;
    stepper.moveStepsAsync(steps, direction, []() {
        std::cout << "Move steps completed." << std::endl;
    });
}

/**
 * @brief Handles moving the stepper motor by angle.
 * @param stepper Reference to the PiStepper object.
 */
void handleMoveAngle(PiStepper &stepper) {
    float angle;
    int direction;
    std::cout << "Enter angle to move: ";
    std::cin >> angle;
    std::cout << "Enter direction (0 for closed, 1 for open): ";
    std::cin >> direction;
    stepper.moveAngle(angle, direction);
    std::cout << "Move angle completed." << std::endl;
}

/**
 * @brief Handles moving the stepper motor to a specific percent open.
 * @param stepper Reference to the PiStepper object.
 */
void handleMoveToPercentOpen(PiStepper &stepper) {
    float percent;
    std::cout << "Enter percent open (0-100): ";
    std::cin >> percent;
    stepper.moveToPercentOpen(percent, []() {
        std::cout << "Move to percent open completed." << std::endl;
    });
}

/**
 * @brief Handles moving the stepper motor to the fully open position.
 * @param stepper Reference to the PiStepper object.
 */
void handleMoveToFullyOpen(PiStepper &stepper) {
    stepper.moveToFullyOpen();
}

/**
 * @brief Handles moving the stepper motor to the fully closed position.
 * @param stepper Reference to the PiStepper object.
 */
void handleMoveToFullyClosed(PiStepper &stepper) {
    stepper.moveToFullyClosed();
}

/**
 * @brief Handles calibrating the stepper motor.
 * @param stepper Reference to the PiStepper object.
 */
void handleCalibrate(PiStepper &stepper) {
    stepper.calibrate();
}

/**
 * @brief Handles emergency stop of the stepper motor.
 * @param stepper Reference to the PiStepper object.
 */
void handleEmergencyStop(PiStepper &stepper) {
    stepper.emergencyStop();
}

/**
 * @brief Handles getting the status of the stepper motor.
 * @param stepper Reference to the PiStepper object.
 */
void handleGetStatus(PiStepper &stepper) {
    std::cout << "Current Step Count: " << stepper.getCurrentStepCount() << std::endl;
    std::cout << "Full Range Count: " << stepper.getFullRangeCount() << std::endl;
    std::cout << "Percent Open: " << stepper.getPercentOpen() << "%" << std::endl;
    std::cout << "Moving: " << (stepper.isMoving() ? "Yes" : "No") << std::endl;
}

/**
 * @brief Displays the menu and handles user input.
 * @param stepper Reference to the PiStepper object.
 */
void displayMenu(PiStepper &stepper) {
    while (true) {
        std::cout << "\nPiStepper Driver Menu:\n";
        std::cout << "1. Move Steps\n";
        std::cout << "2. Move Angle\n";
        std::cout << "3. Move to Percent Open\n";
        std::cout << "4. Move to Fully Open\n";
        std::cout << "5. Move to Fully Closed\n";
        std::cout << "6. Calibrate\n";
        std::cout << "7. Emergency Stop\n";
        std::cout << "8. Get Status\n";
        std::cout << "9. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                handleMoveSteps(stepper);
                break;
            case 2:
                handleMoveAngle(stepper);
                break;
            case 3:
                handleMoveToPercentOpen(stepper);
                break;
            case 4:
                handleMoveToFullyOpen(stepper);
                break;
            case 5:
                handleMoveToFullyClosed(stepper);
                break;
            case 6:
                handleCalibrate(stepper);
                break;
            case 7:
                handleEmergencyStop(stepper);
                break;
            case 8:
                handleGetStatus(stepper);
                break;
            case 9:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }

        // Small delay to avoid rapid looping in case of accidental input
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    PiStepper stepper(27, 17, 22, 200, 1); // Initialize PiStepper with 1x microstepping
    displayMenu(stepper);
    return 0;
}
