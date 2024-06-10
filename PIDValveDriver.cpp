#include "TSI40xx.h"
#include "PiStepper.h"
#include "PIDValveController.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <termios.h>
#include <unistd.h>

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

// Function to handle user input
void userInputHandler(PIDValveController* controller, std::atomic<bool>& running) {
    setNonBlockingMode();

    while (running) {
        if (std::cin.peek() != EOF) {
            std::string command;
            std::cin >> command;

            if (command == "setpoint") {
                double setPoint;
                std::cout << "Enter new setpoint: ";
                std::cin >> setPoint;
                controller->setSetPoint(setPoint);
                std::cout << "Setpoint updated to " << setPoint << std::endl;
            } else if (command == "pid") {
                double kp, ki, kd;
                std::cout << "Enter new PID constants (kp ki kd): ";
                std::cin >> kp >> ki >> kd;
                controller->setPIDConstants(kp, ki, kd);
                std::cout << "PID constants updated to kp=" << kp << ", ki=" << ki << ", kd=" << kd << std::endl;
            } else if (command == "stop") {
                controller->stopControlLoop();
                running = false;
                std::cout << "Control loop stopped." << std::endl;
                break;
            } else if (command == "status") {
                double kp, ki, kd;
                controller->getPIDConstants(kp, ki, kd);
                std::cout << "Current Flow: " << controller->getCurrentFlow() << std::endl;
                std::cout << "Setpoint: " << controller->getSetPoint() << std::endl;
                std::cout << "PID constants: kp=" << kp << ", ki=" << ki << ", kd=" << kd << std::endl;
            } else {
                std::cout << "Invalid command. Available commands: setpoint, pid, stop, status" << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    restoreBlockingMode();
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
        double initialSetPoint = 80.0; // Example initial setpoint
        double kp = 1.0, ki = 0.1, kd = 0.01; // Example PID constants
        PIDValveController pidController(&flowMeter, &stepperMotor, initialSetPoint, kp, ki, kd);

        // Start the control loop
        pidController.startControlLoop();

        // Handle user input in a separate thread
        std::thread inputThread(userInputHandler, &pidController, std::ref(running));
        inputThread.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        running = false;
    }

    return 0;
}
