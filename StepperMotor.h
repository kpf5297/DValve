
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

    // function to setup event detection for both NO and NC contacts
void StepperMotor::setupLimitSwitchEvents(gpiod_chip *chip, int pinNO1, int pinNC1, int pinNO2,
 int pinNC2, gpiod_line **lineNO1, gpiod_line **lineNC1, gpiod_line **lineNO2, gpiod_line **lineNC2,
  const std::string& consumerNO1, const std::string& consumerNC1, const std::string& consumerNO2, 
  const std::string& consumerNC2) {


    // ISR for handling limit switch events
    void handleLimitSwitchEvent(gpiod_line *line, const char* eventDescription);

    // In main loop or dedicated thread for handling GPIO events
    // Example for a single limit switch with both NO and NC contacts
    // void StepperMotor::gpiod_line_event event;
    // if (gpiod_line_event_wait(lineNO, NULL) > 0) {
    //     handleLimitSwitchEvent(lineNO, "NO Contact");
    // }
    // if (gpiod_line_event_wait(lineNC, NULL) > 0) {
    //     handleLimitSwitchEvent(lineNC, "NC Contact");
    // }


};

#endif // STEPPER_MOTOR_H
