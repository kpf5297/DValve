/*

To compile: g++ -o mttest motorTravelDriver.cpp DPSTSwitch.cpp PiStepper.cpp -lgpiod

*/

#include "DPSTSwitch.h"
#include "PiStepper.h"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <atomic>

std::atomic<bool> stopMotor(false);

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int TOP_NO_PIN = 16;
int TOP_NC_PIN = 12;
int BOTTOM_NO_PIN = 20;
int BOTTOM_NC_PIN = 21;

void moveMotor(PiStepper& stepper, DPSTSwitch& topSwitch, DPSTSwitch& bottomSwitch) {
    while (!stopMotor) {
        // Check if the top switch is pressed
        if (topSwitch.readNOPin()) {
            // Move the motor down
            stepper.moveSteps(1, 0);
        }

        // Check if the bottom switch is pressed
        if (bottomSwitch.readNOPin()) {
            // Move the motor up
            stepper.moveSteps(1, 1);
        }

        // Sleep for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);
    DPSTSwitch bottomSwitch("gpiochip0", BOTTOM_NO_PIN, BOTTOM_NC_PIN);
    DPSTSwitch topSwitch("gpiochip0", TOP_NO_PIN, TOP_NC_PIN);
    
    // Loop to move motor while checking for switch presses non thread
    while (true) { 
        // Check if the top switch is pressed
        if (topSwitch.readNOPin()) {
            // Move the motor down
            stepper.moveSteps(1, 0);
        }

        // Check if the bottom switch is pressed
        if (bottomSwitch.readNOPin()) {
            // Move the motor up
            stepper.moveSteps(1, 1);
        }

        // Sleep for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    

    return 0;
}
