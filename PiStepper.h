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

    The pins are mapped as follows:

    - Step pin:         27 (Active Low) low-level pulse should be > 4us
    - Direction pin:    17 (Active Low) input is high level if this terminal is not connected
    - Enable pin:       22 (Active Low) active low-level input shuts down power supply to the motor

    This library uses the GPIOD library to control the GPIO pins.

*/
// PiStepper.h
#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>

class PiStepper {
public:
    // doxygen comments
    /**
     * @brief Construct a new PiStepper object
     * 
     * @param stepPin The GPIO pin number for the step pin
     * @param dirPin The GPIO pin number for the direction pin
     * @param enablePin The GPIO pin number for the enable pin
     * @param stepsPerRevolution The number of steps per revolution for the stepper motor
     * @param microstepping The microstepping value for the stepper motor
     */
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);
    ~PiStepper();

    /**
     * @brief Set the speed of the stepper motor in RPM
     * 
     * @param speed The speed of the stepper motor in RPM
     * @return void
     */
    void setSpeed(float speed);

    /**
     * @brief Set the acceleration of the stepper motor in RPM/s
     * 
     * @param acceleration The acceleration of the stepper motor in RPM/s
     * @return void
     */
    void setAcceleration(float acceleration);

    /**
     * @brief Enable the stepper motor
     * 
     * @return void
     */
    void enable();

    /**
     * @brief Disable the stepper motor
     * 
     * @return void
     */
    void disable();

    /**
     * @brief Move the stepper motor a specified number of steps in a specified direction
     * 
     * @param steps The number of steps to move the stepper motor
     * @param direction The direction to move the stepper motor (0 or 1)
     * @return void
     */    
    void moveSteps(int steps, int direction);

    /**
     * @brief Move the stepper motor a specified number of steps in a specified direction with acceleration
     * 
     * @param steps The number of steps to move the stepper motor
     * @param direction The direction to move the stepper motor (0 or 1)
     * @return void
    */
    void accelMoveSteps(int steps, int direction);

    /**
     * @brief Move the stepper motor a specified angle in a specified direction
     * 
     * @param angle The angle to move the stepper motor
     * @param direction The direction to move the stepper motor (0 or 1)
     * @return void
     */
    void moveAngle(float angle, int direction);

    /**
     * @brief Set the microstepping value for the stepper motor
     * 
     * @param microstepping The microstepping value for the stepper motor
     * @return void
     */
    void setMicrostepping(int microstepping);

    /**
     * @brief Delay for a specified number of seconds
     * 
     * @param seconds The number of seconds to delay
     * @return void
     */
    void delay(float seconds);

private:
    // Private member variables

    // GPIO

    /**
     * @brief The GPIO pin number for the step pin
     * 
     */
    int _stepPin;

    /**
     * @brief The GPIO pin number for the direction pin
     * 
     */
    int _dirPin;

    /**
     * @brief The GPIO pin number for the enable pin
     * 
     */
    int _enablePin;

    /**
     * @brief The number of steps per revolution for the stepper motor
     * 
     */
    int _stepsPerRevolution;

    /**
     * @brief The microstepping value for the stepper motor
     * 
     */
    int _microstepping;

    /**
     * @brief The speed of the stepper motor in RPM
     * 
     */
    float _speed;

    /**
     * @brief The acceleration of the stepper motor in RPM/s
     * 
     */
    float _acceleration;

    /**
     * @brief The GPIO chip
    */
    gpiod_chip *chip;

    /**
     * @brief The GPIO line for the step pin
     * 
     */
    gpiod_line *step_signal;

    /**
     * @brief The GPIO line for the direction pin
     * 
     */
    gpiod_line *dir_signal;

    /**
     * @brief The GPIO line for the enable pin
     * 
     */
    gpiod_line *enable_signal;

    // Private member functions
    /**
     * @brief Convert steps to angle
     * 
     * @param steps The number of steps to convert to angle
     * @return float The angle in degrees
     */
    float stepsToAngle(int steps);
};

#endif

