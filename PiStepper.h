/*
    PiStepper - A simple library for controlling stepper motors on a Raspberry Pi with acceleration.

    This library is used to control stepper motors using the StepperOnline Integrated Step Driver ISD04.

    Driver Details:
    Note:
    (1) Input is considered high level if this terminal is not connected.
    (2) Low-level pulse duration should > 4μs.
    (3) An active low-level input shuts down power supply to the motor. High-level input or 
        left open makes the controller fully working. When awaken from shutdown mode, wait 1 millisecond 
        before sending pulse.

    This library uses the GPIOD library to control the GPIO pins.
*/
// PiStepper.h
#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>

class PiStepper {
public:
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);
    ~PiStepper();
    void setSpeed(float speed);
    void setAcceleration(float acceleration);
    void enable();
    void disable();
    void moveSteps(int steps, int direction);
    void moveAngle(float angle, int direction);
    void setMicrostepping(int microstepping);
    void delay(float seconds);
    int getSteps();
    int getDirection();
    void setSteps(int steps);
    void setDirection(int direction);
    void runMotor();
    bool move(int steps);
    void setHome();


private:
    int _relativeStep;
    int _homeStep;
    int _currentDirection;
    int _remainingSteps;
    int _stepPin;
    int _dirPin;
    int _enablePin;
    int _stepsPerRevolution;
    int _microstepping;
    float _speed;
    float _acceleration;
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    float stepsToAngle(int steps);
    int angleToSteps(float angle);
};

#endif

