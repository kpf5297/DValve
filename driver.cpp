// Driver.cpp

#include "StepperMotor.h"

#include <iostream>
#include <thread>
#include <chrono>

gpiod_chip* gpiochip0;

// Function for handling GPIO events
void gpioEventLoop(gpiod_line* lineNO1, gpiod_line* lineNC1, gpiod_line* lineNO2, gpiod_line* lineNC2, StepperMotor& motor) {
    while (true) {
        if (gpiod_line_event_wait(lineNO1, NULL) > 0) {
            motor.handleLimitSwitchEvent(lineNO1, "NO Contact 1");
        }
        if (gpiod_line_event_wait(lineNC1, NULL) > 0) {
            motor.handleLimitSwitchEvent(lineNC1, "NC Contact 1");
        }
        if (gpiod_line_event_wait(lineNO2, NULL) > 0) {
            motor.handleLimitSwitchEvent(lineNO2, "NO Contact 2");
        }
        if (gpiod_line_event_wait(lineNC2, NULL) > 0) {
            motor.handleLimitSwitchEvent(lineNC2, "NC Contact 2");
        }
    }
}

// Function for performing other tasks
void otherTasksLoop(StepperMotor& motor) {
    while (true) {
        // Perform other tasks

        // Move the motor 200 steps
        motor.move(200);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for a while
    }
}

int main() {

    // Set up all the GPIO pins and initialize the stepper motor
    gpiochip0 = gpiod_chip_open_by_name("gpiochip0");
    gpiod_line* lineNO1;
    gpiod_line* lineNC1;
    gpiod_line* lineNO2;
    gpiod_line* lineNC2;
    StepperMotor motor(17, 22, 23, "gpiochip0");
    motor.setupLimitSwitchEvents(gpiochip0, 24, 25, 26, 27, &lineNO1, &lineNC1, &lineNO2, &lineNC2, "NO1", "NC1", "NO2", "NC2");

    // Using threads for handling GPIO events and other tasks
    std::thread gpioEventThread(gpioEventLoop, lineNO1, lineNC1, lineNO2, lineNC2, motor);
    std::thread otherTasksThread(otherTasksLoop, motor);

    // Wait for threads to finish
    gpioEventThread.join();
    otherTasksThread.join();
    

    gpiod_chip_close(gpiochip0);
    return 0;
}
