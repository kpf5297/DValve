#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>

// GPIO pin for the bottom limit switch
#define LIMIT_SWITCH_BOTTOM_PIN 21

class PiStepper {
public:
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);
    ~PiStepper();

    // Setters
    void setSpeed(float speed); // Set the speed of the stepper motor in RPM
    void setAcceleration(float acceleration); // Set the acceleration of the stepper motor in RPM/s
    void setMicrostepping(int microstepping); // Set the microstepping value for the stepper motor

    // Stepper control
    void enable(); // Enable the stepper motor
    void disable(); // Disable the stepper motor
    void moveSteps(int steps, int direction); // Move the stepper motor a specified number of steps in a specified direction
    void moveAngle(float angle, int direction); // Move the stepper motor a specified angle in a specified direction
    
    // Homing
    void homeMotor(); // Move the motor towards a limit switch to define a home position

    // Position tracking
    int getCurrentStepCount() const; // Get the current step count relative to the starting position

private:
    // GPIO pin assignments
    int _stepPin;
    int _dirPin;
    int _enablePin;
    int _stepsPerRevolution;
    int _microstepping;
    float _speed;
    float _acceleration;
    int _currentStepCount; // Tracks the current step position relative to the starting point

    // GPIO chip and line pointers
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    gpiod_line *limit_switch_bottom;
    // gpiod_line *limit_switch_top;

    // Private methods
    float stepsToAngle(int steps); // Convert steps to angle
    std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h