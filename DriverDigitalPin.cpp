
/*

    g++ -o gpio_test DriverDigitalPin.cpp DigitalPin.cpp -lgpiod


*/

#include <iostream>
#include <string>
#include "DigitalPin.h"

class DriverDigitalPin {
public:
    DriverDigitalPin() 
        : outputPin(17, DigitalPin::Direction::Output, "LED"), // Example pin for LED
          inputPin(27, DigitalPin::Direction::Input, "Button") // Example pin for a button
    {}

    void run() {
        std::string input;
        std::cout << "Enter 'r' to read from input pin, 'w' to toggle output pin, 'q' to quit: ";
        while (std::getline(std::cin, input) && input != "q") {
            handleInput(input);
            std::cout << "Enter 'r' to read from input pin, 'w' to toggle output pin, 'q' to quit: ";
        }
    }

private:
    DigitalPin outputPin;
    DigitalPin inputPin;

    void handleInput(const std::string& input) {
        if (input == "r") {
            try {
                bool state = inputPin.read();
                std::cout << "Input pin state: " << (state ? "HIGH" : "LOW") << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        } else if (input == "w") {
            static bool state = false;
            state = !state;
            try {
                outputPin.write(state);
                std::cout << "Output pin set to: " << (state ? "HIGH" : "LOW") << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        }
    }
};

int main() {
    try {
        DriverDigitalPin driver;
        driver.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
