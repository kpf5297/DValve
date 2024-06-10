/*
    g++ -o pid_valve_controller PIDValveDriver.cpp TSI40xx.cpp PiStepper.cpp PIDValveController.cpp SerialPortManager.cpp -lgpiod -pthread
*/

#include "TSI40xx.h"
#include "PiStepper.h"
#include "PIDValveController.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <unistd.h>

const int STABLE_READINGS_COUNT = 5; // Number of stable readings required
const double FLOW_TOLERANCE = 5.0;   // Tolerance for flow rate

// Function to set terminal to non-blocking mode
void setNonBlockingMode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Function to restore terminal to blocking mode
void restoreBlockingMode() {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    ttystate.c_lflag |= ICANON;
    ttystate.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

// Function to handle menu options
void displayMenu() {
    std::cout << "Menu Options:\n";
    std::cout << "1. Set target flow rate\n";
    std::cout << "2. Adjust PID constants\n";
    std::cout << "3. Check status\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

void handleMenuOption(PIDValveController* controller, std::atomic<bool>& running) {
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1: {
            double targetFlow;
            std::cout << "Enter target flow rate: ";
            std::cin >> targetFlow;
            bool result = controller->adjustFlowToTarget(targetFlow, FLOW_TOLERANCE, STABLE_READINGS_COUNT);
            if (result) {
                std::cout << "Flow adjusted to target and is stable.\n";
            }
            break;
        }
        case 2: {
            double kp, ki, kd;
            std::cout << "Enter new PID constants (kp ki kd): ";
            std::cin >> kp >> ki >> kd;
            controller->setPIDConstants(kp, ki, kd);
            std::cout << "PID constants updated to kp=" << kp << ", ki=" << ki << ", kd=" << kd << "\n";
            break;
        }
        case 3: {
            double kp, ki, kd;
            controller->getPIDConstants(kp, ki, kd);
            std::cout << "Current Flow: " << controller->getCurrentFlow() << "\n";
            std::cout << "Setpoint: " << controller->getSetPoint() << "\n";
            std::cout << "PID constants: kp=" << kp << ", ki=" << ki << ", kd=" << kd << "\n";
            break;
        }
        case 4:
            running = false;
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
    }
}

int main() {
    std::atomic<bool> running(true);
    try {
        // Initialize the TSI40xx flow meter
        TSI40xx flowMeter;

        // Initialize the PiStepper motor controller
        PiStepper stepperMotor(27, 17, 22); // Example GPIO pins, change as needed

        // Calibrate the stepper motor
        stepperMotor.calibrate();

        // Initialize the PIDValveController
        double initialSetPoint = 100.0; // Example initial setpoint
        double kp = 2.0, ki = 0.05, kd = 0.1; // Example PID constants, these may need further tuning
        PIDValveController pidController(&flowMeter, &stepperMotor, initialSetPoint, kp, ki, kd);

        while (running) {
            displayMenu();
            handleMenuOption(&pidController, running);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        running = false;
    }

    return 0;
}
