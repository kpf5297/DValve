#include "PiStepper.h"
#include <iostream>
#include <string>

int main() {
    // Initialize the PiStepper object with GPIO pins for the step, direction, and enable lines
    // Adjust these pin numbers according to your hardware setup
    PiStepper motor(27, 17, 22, 200, 1);

    // Set motor speed and acceleration (optional, can adjust as needed)
    motor.setSpeed(60); // Set motor speed to 60 RPM
    motor.setAcceleration(100); // Set motor acceleration to 100 RPM/s

    std::cout << "Motor is ready. Enter 'q' to quit." << std::endl;

    std::string input;
    float angle;
    int direction;

    while (true) {
        std::cout << "Enter angle in degrees (float) and direction (0 for CCW, 1 for CW): ";
        std::cin >> input;

        if (input == "q" || input == "Q") {
            std::cout << "Quitting program." << std::endl;
            break;
        }

        try {
            angle = std::stof(input);
            std::cin >> direction;

            if (direction != 0 && direction != 1) {
                std::cout << "Invalid direction. Use 0 for CCW or 1 for CW." << std::endl;
                continue;
            }

            std::cout << "Moving motor " << angle << " degrees in direction " << direction << "." << std::endl;
            motor.moveAngle(angle, direction);
        }
        catch (const std::invalid_argument& ia) {
            std::cerr << "Invalid input. Please enter a valid angle and direction." << std::endl;
            // Clear and ignore the rest of the line to prevent infinite loops
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    return 0;
}
