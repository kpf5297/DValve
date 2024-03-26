/**
 * @file motortest.cpp
 * @brief Test program for the PiStepper class
 * 
 * This program tests the PiStepper class by moving the stepper motor
 * a specified number of steps in a specified direction.
 * 
 * To compile: g++ -o mtest motortest.cpp PiStepper.cpp -lgpiod -lpthread
 * 
*/

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <pthread.h>
#include "PiStepper.h"

int STEP_PIN = 27;
int DIR_PIN = 17;
int ENABLE_PIN = 22;
int STEPS_PER_REVOLUTION = 200;
int MICROSTEPPING = 1;

// Function to run the motor in a separate thread
void* runMotor(void* arg) {
    PiStepper* stepper = static_cast<PiStepper*>(arg);

    // Move the stepper motor
    stepper->moveSteps(stepper->getSteps(), stepper->getDirection());

    return nullptr;
}

int main() {
    // Create a PiStepper object
    PiStepper stepper(STEP_PIN, DIR_PIN, ENABLE_PIN, STEPS_PER_REVOLUTION, MICROSTEPPING);

    // Set the speed and acceleration
    stepper.setSpeed(20);
    stepper.setAcceleration(80);

    // Get user input for number of steps and direction
    int numSteps;
    int direction;
    std::cout << "Enter the number of steps: ";
    std::cin >> numSteps;
    std::cout << "Enter the direction (0 for reverse, 1 for forward): ";
    std::cin >> direction;

    // Set the number of steps and direction in the stepper object
    stepper.setSteps(numSteps);
    stepper.setDirection(direction);

    // Create a thread to run the motor
    pthread_t motorThread;
    pthread_create(&motorThread, nullptr, runMotor, &stepper);

    // Wait for the motor thread to finish
    pthread_join(motorThread, nullptr);

    return 0;
}