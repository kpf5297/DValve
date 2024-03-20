/**
 * @file motortest.cpp
 * @brief Test program for the PiStepper class
 * 
 * This program tests the PiStepper class by moving the stepper motor
 * a specified number of steps in a specified direction.
 * 
 * To compile: g++ -o mtest motortest.cpp PiStepper.cpp -lgpiod
 * 
*/

#include <iostream>
#include <unistd.h>
#include <cmath>
#include "PiStepper.h"

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

int main() {
    // Create a PiStepper object
    PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);

    // Set the speed and acceleration
    // stepper.setSpeed(20);
    // stepper.setAcceleration(80);



    for (int i = 0; i < 1; i++) {
        // Move the stepper motor 200 steps in the forward direction
        stepper.moveSteps(200, 1);

        // Move the stepper motor 200 steps in the reverse direction
        stepper.moveSteps(200, 0);
    }

    usleep(5000000);



    return 0;
}