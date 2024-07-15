#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>
#include <functional>

#define LIMIT_SWITCH_BOTTOM_PIN 21
#define LIMIT_SWITCH_TOP_PIN 20
#define STEPS_PER_REVOLUTION 200
#define MICROSTEPPING 1
#define DEFAULT_SPEED 20
#define DEFAULT_ACCELERATION 80
#define STEP_PIN 17
#define DIR_PIN 27
#define ENABLE_PIN 22
#define MAX_SPEED 50

class PiStepper {
public:
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping);
    PiStepper();
    ~PiStepper();

    // Setters
    void setSpeed(float speed); // Set the speed of the stepper motor in RPM
    void setAcceleration(float acceleration); // Set the acceleration of the stepper motor in RPM/s
    void setMicrostepping(int microstepping); // Set the microstepping value for the stepper motor

    // Getters
    int getStepsPerRevolution() const; // Get the number of steps per revolution
    int getMicrostepping() const; // Get the microstepping value
    float getSpeed() const; // Get the speed of the stepper motor in RPM
    float getAcceleration() const; // Get the acceleration of the stepper motor in RPM/s

    // Stepper control
    void enable(); // Enable the stepper motor
    void disable(); // Disable the stepper motor
    void moveSteps(int steps, int direction); // Move the stepper motor a specified number of steps in a specified direction
    void moveAngle(float angle, int direction); // Move the stepper motor a specified angle in a specified direction
    void moveStepsAsync(int steps, int direction, std::function<void()> callback); // Move steps asynchronously
    void stopMovement(); // Stop the current movement
    void emergencyStop(); // Perform an emergency stop

    // Homing and calibration
    void calibrate(); // Calibrate the motor using limit switches

    // Position tracking
    int getCurrentStepCount() const; // Get the current step count relative to the starting position
    int getFullRangeCount() const; // Get the full range count determined during calibration
    float getPercentOpen() const; // Get the current position as a percentage of the full range
    bool isMoving() const; // Check if the motor is currently moving

    // Move to specific positions
    void moveToPercentOpen(float percent, std::function<void()> callback); // Move to a specified percentage open
    void moveToFullyOpen(); // Move to the fully open position
    void moveToFullyClosed(); // Move to the fully closed position

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
    int _fullRangeCount; // The number of steps from fully closed to fully open
    bool _isMoving; // Flag to indicate if the motor is moving
    bool _isCalibrated; // Flag to indicate if the motor has been calibrated



    // GPIO chip and line pointers
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    gpiod_line *limit_switch_bottom;
    gpiod_line *limit_switch_top;

    // Private methods
    float stepsToAngle(int steps) const; // Convert steps to angle
    mutable std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h
