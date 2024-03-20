
#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <gpiod.h>
#include <chrono>
#include <thread>

class StepperMotor {
public:
    StepperMotor(int enablePin, int pulsePin, int directionPin, const std::string& chipName = "gpiochip0");
    ~StepperMotor();

    void setSpeed(long rpm);
    void setAcceleration(long rpm_per_second);
    void setMicrostepping(int microsteps);
    void move(int steps);
    void stop();

private:
    gpiod_chip* chip;
    gpiod_line* enableLine;
    gpiod_line* pulseLine;
    gpiod_line* directionLine;

    long stepDelay; // Microseconds
    long acceleration;
    int microstepping;

    void initializeGPIO(int pin, gpiod_line** line);
    void setDirection(bool direction);
    void enableMotor(bool enable);
    void step();
    std::chrono::microseconds calculateStepDelay(int steps);
};

#endif // STEPPER_MOTOR_H
