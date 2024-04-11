#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>

// GPIO pin for the bottom limit switch
#define LIMIT_SWITCH_BOTTOM_PIN 6

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

    // Private methods
    float stepsToAngle(int steps); // Convert steps to angle
    std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h


// /*
//     PiStepper - A simple library for controlling stepper motors on a Raspberry Pi with acceleration.

//     This library is used to control stepper motors using the StepperOnline Integrated Step Driver ISD04.

//     Driver Details:
//     Note:
//     (1) Input is considered high level if this terminal is not connected.
//     (2) Low-level pulse duration should > 4μs.
//     (3) An active low-level input shuts down power supply to the motor. High-level input or 
//         left open makes the controller fully working. When awaken from shutdown mode, wait 1 millisecond 
//         before sending pulse.

//     The pins are mapped as follows:

//     - Step pin:         27 (Active Low) low-level pulse should be > 4us
//     - Direction pin:    17 (Active Low) input is high level if this terminal is not connected
//     - Enable pin:       22 (Active Low) active low-level input shuts down power supply to the motor

//     This library uses the GPIOD library to control the GPIO pins.

//     I need to add limit swtiches to the design two of them, one to the top lmit of movement and one 
//     to the bottom,  This will consist of two inputs/GPIO pins for each switch, both n?O and N/C.  
//     The activation of either of the normally closed will trigger one of two corresponding ISR's 
//     that also need to be created.

// */
// // PiStepper.h
// #ifndef PiStepper_h
// #define PiStepper_h

// #include <gpiod.h>
// #include <iostream>
// #include <mutex>

// class PiStepper {
// public:
//     // doxygen comments
//     /**
//      * @brief Construct a new PiStepper object
//      * 
//      * @param stepPin The GPIO pin number for the step pin
//      * @param dirPin The GPIO pin number for the direction pin
//      * @param enablePin The GPIO pin number for the enable pin
//      * @param stepsPerRevolution The number of steps per revolution for the stepper motor
//      * @param microstepping The microstepping value for the stepper motor
//      */
//     PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);
//     ~PiStepper();

//     /**
//      * @brief Set the speed of the stepper motor in RPM
//      * 
//      * @param speed The speed of the stepper motor in RPM
//      * @return void
//      */
//     void setSpeed(float speed);

//     /**
//      * @brief Set the acceleration of the stepper motor in RPM/s
//      * 
//      * @param acceleration The acceleration of the stepper motor in RPM/s
//      * @return void
//      */
//     void setAcceleration(float acceleration);

//     /**
//      * @brief Enable the stepper motor
//      * 
//      * @return void
//      */
//     void enable();

//     /**
//      * @brief Disable the stepper motor
//      * 
//      * @return void
//      */
//     void disable();

//     /**
//      * @brief Move the stepper motor a specified number of steps in a specified direction
//      * 
//      * @param steps The number of steps to move the stepper motor
//      * @param direction The direction to move the stepper motor (0 or 1)
//      * @return void
//      */    
//     void moveSteps(int steps, int direction);

//     /**
//      * @brief Move the stepper motor a specified number of steps in a specified direction with acceleration
//      * 
//      * @param steps The number of steps to move the stepper motor
//      * @param direction The direction to move the stepper motor (0 or 1)
//      * @return void
//     */
//     void accelMoveSteps(int steps, int direction);

//     /**
//      * @brief Move the stepper motor a specified angle in a specified direction
//      * 
//      * @param angle The angle to move the stepper motor
//      * @param direction The direction to move the stepper motor (0 or 1)
//      * @return void
//      */
//     void moveAngle(float angle, int direction);

//     /**
//      * @brief Set the microstepping value for the stepper motor
//      * 
//      * @param microstepping The microstepping value for the stepper motor
//      * @return void
//      */
//     void setMicrostepping(int microstepping);

//     /**
//      * @brief Delay for a specified number of seconds
//      * 
//      * @param seconds The number of seconds to delay
//      * @return void
//      */
//     void delay(float seconds);

//     int getSteps();

//     int getDirection();

//     void setSteps(int steps);

//     void setDirection(int direction);

//     void runMotor();


//     bool move(int steps);

// private:

//     int _currentDirection;
//     int _remainingSteps;
//     int _stepPin;
//     int _dirPin;
//     int _enablePin;
//     int _stepsPerRevolution;
//     int _microstepping;
//     float _speed;
//     float _acceleration;
//     gpiod_chip *chip;
//     gpiod_line *step_signal;
//     gpiod_line *dir_signal;
//     gpiod_line *enable_signal;
//     float stepsToAngle(int steps);
//     std::mutex gpioMutex;
// };

// #endif

// // PiStepper.cpp
// #include "PiStepper.h"
// #include <iostream>
// #include <unistd.h>
// #include <cmath>

// /**
//  * @brief Construct a new PiStepper object
//  * 
//  * @param stepPin The GPIO pin number for the step pin
//  * @param dirPin The GPIO pin number for the direction pin
//  * @param enablePin The GPIO pin number for the enable pin
//  * @param stepsPerRevolution The number of steps per revolution for the stepper motor
//  * @param microstepping The microstepping value for the stepper motor
//  */
// PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping) {
//     _stepPin = stepPin;
//     _dirPin = dirPin;
//     _enablePin = enablePin;
//     _stepsPerRevolution = stepsPerRevolution;
//     _microstepping = microstepping;
//     _speed = 20;
//     _acceleration = 80;

//     // Initialize the GPIO pins
//     chip = gpiod_chip_open("/dev/gpiochip0");
//     step_signal = gpiod_chip_get_line(chip, _stepPin);
//     dir_signal = gpiod_chip_get_line(chip, _dirPin);
//     enable_signal = gpiod_chip_get_line(chip, _enablePin);

//     // Configure the GPIO pins
//     gpiod_line_request_output(step_signal, "PiStepper", 0);
//     gpiod_line_request_output(dir_signal, "PiStepper", 0);
//     gpiod_line_request_output(enable_signal, "PiStepper", 0);

//     // Disable the stepper motor
//     disable();
// }


// PiStepper::~PiStepper() {
//     // Close the GPIO pins
//     gpiod_line_release(step_signal);
//     gpiod_line_release(dir_signal);
//     gpiod_line_release(enable_signal);
//     gpiod_chip_close(chip);
// }


// void PiStepper::setSpeed(float speed) {
//     _speed = speed;
// }


// void PiStepper::setAcceleration(float acceleration) {
//     _acceleration = acceleration;
// }


// void PiStepper::enable() {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     gpiod_line_set_value(enable_signal, 1);
// }


// void PiStepper::disable() {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     gpiod_line_set_value(enable_signal, 0);
// }

// void PiStepper::moveSteps(int steps, int direction) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//     _remainingSteps = steps;
//     _currentDirection = direction;
    
//     // Set the direction
//     gpiod_line_set_value(dir_signal, direction);

//     // Define the starting speed, maximum speed, and acceleration
//     double startSpeed = 0.1; // Adjust as needed
//     double maxSpeed = 1.2; // Adjust as needed
//     double acceleration = _acceleration; // Adjust as needed

//     // Calculate the number of steps for acceleration and deceleration
//     int accelSteps = (maxSpeed - startSpeed) / acceleration;
//     int decelSteps = steps - accelSteps;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Acceleration
//     for (int i = 0; i < accelSteps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Calculate the current speed
//         double speed = startSpeed + i * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Constant speed
//     for (int i = accelSteps; i < decelSteps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Deceleration
//     for (int i = decelSteps; i < steps; i++) {
//         std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block
//         // Calculate the current speed
//         double speed = maxSpeed - (i - decelSteps) * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }
// }

// void PiStepper::accelMoveSteps(int steps, int direction) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block

//     _remainingSteps = steps;
//     _currentDirection = direction;
    

//     // Set the direction
//     gpiod_line_set_value(dir_signal, direction);

//     // Calculate the delay between steps
//     float delay = 1 / (_speed * _stepsPerRevolution * _microstepping);

//     // Calculate the number of steps to accelerate
//     int stepsToAccel = (_speed * _speed) / (2 * _acceleration * delay);

//     // Calculate the number of steps to decelerate
//     int stepsToDecel = stepsToAccel + stepsToAccel / 2;

//     // Calculate the number of steps to run at constant speed
//     int stepsAtSpeed = steps - stepsToDecel - stepsToAccel;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Accelerate
//     for (int i = 0; i < stepsToAccel; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;

//     }

//     // Run at constant speed
//     for (int i = 0; i < stepsAtSpeed; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Decelerate
//     for (int i = 0; i < stepsToDecel; i++) {
//         gpiod_line_set_value(step_signal, 1);
//         usleep(delay * 1000000);
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delay * 1000000);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Disable the stepper motor
//     disable();
// }

// void PiStepper::moveAngle(float angle, int direction) {

//     _currentDirection = direction;
    

//     int steps = round(angle * _stepsPerRevolution * _microstepping / 360);
//     moveSteps(steps, direction);
// }

// void PiStepper::setMicrostepping(int microstepping) {
    

//     _microstepping = microstepping;
// }

// void PiStepper::delay(float seconds) {
    

//     usleep(seconds * 1000000);
// }

// bool PiStepper::move(int steps) {
//     // std::lock_guard<std::mutex> lock(gpioMutex); // Lock the mutex for the scope of this block

//     _remainingSteps = steps;

//     // Define the starting speed, maximum speed, and acceleration
//     double startSpeed = 0.1; // Adjust as needed
//     double maxSpeed = 1.0; // Adjust as needed
//     double acceleration = _acceleration; // Adjust as needed

//     // Calculate the number of steps for acceleration and deceleration
//     int accelSteps = (maxSpeed - startSpeed) / acceleration;
//     int decelSteps = steps - accelSteps;

//     // Enable the stepper motor
//     enable();

//     // Wait for the stepper motor to enable
//     usleep(1000000);

//     // Acceleration
//     for (int i = 0; i < accelSteps; i++) {
//         // Calculate the current speed
//         double speed = startSpeed + i * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Constant speed
//     for (int i = accelSteps; i < decelSteps; i++) {
//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(1000000 / (maxSpeed * _stepsPerRevolution * _microstepping));
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     // Deceleration
//     for (int i = decelSteps; i < steps; i++) {
//         // Calculate the current speed
//         double speed = maxSpeed - (i - decelSteps) * acceleration;

//         // Calculate the delay between steps
//         double delayTime = 1000000 / (speed * _stepsPerRevolution * _microstepping);

//         // Move one step
//         gpiod_line_set_value(step_signal, 0);
//         usleep(delayTime);
//         gpiod_line_set_value(step_signal, 1);

//         // Decrement the remaining steps
//         _remainingSteps--;
//     }

//     return true;
// }

// int PiStepper::getSteps() {
//     return _remainingSteps;
// }

// int PiStepper::getDirection() {
//     return _currentDirection;
// }

// void PiStepper::setDirection(int direction) {
//     _currentDirection = direction;
// }

// void PiStepper::runMotor() {
//     // Move the stepper motor
//     moveSteps(getSteps(), getDirection());
// }

// void PiStepper::setSteps(int steps) {
//     _remainingSteps = steps;
// }

