#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <functional>

/**
 * @class PiStepper
 * @brief A class for controlling a stepper motor using GPIO on a Raspberry Pi.
 * 
 * This class provides methods to control a stepper motor, including moving the motor by steps or angles, 
 * setting speed and acceleration, and calibrating the motor using limit switches.
 */
class PiStepper {
public:
    /**
     * @brief Constructor for PiStepper.
     * @param stepPin The GPIO pin number for the step signal.
     * @param dirPin The GPIO pin number for the direction signal.
     * @param enablePin The GPIO pin number for the enable signal.
     * @param stepsPerRevolution The number of steps per full revolution of the motor.
     * @param microstepping The microstepping value for the motor.
     */
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);

    /**
     * @brief Destructor for PiStepper.
     */
    ~PiStepper();

    /**
     * @brief Sets the speed of the stepper motor.
     * @param speed The speed in RPM.
     */
    void setSpeed(float speed);

    /**
     * @brief Sets the maximum speed of the stepper motor.
     * @param maxSpeed The maximum speed in RPM.
     */
    void setMaxSpeed(float maxSpeed);

    /**
     * @brief Sets the acceleration of the stepper motor.
     * @param acceleration The acceleration in RPM/s.
     */
    void setAcceleration(float acceleration);

    /**
     * @brief Enables the stepper motor.
     */
    void enable();

    /**
     * @brief Disables the stepper motor.
     */
    void disable();

    /**
     * @brief Moves the stepper motor a specified number of steps.
     * @param steps The number of steps to move.
     * @param direction The direction to move (0 for closed, 1 for open).
     */
    void moveSteps(int steps, int direction);

    /**
     * @brief Moves the stepper motor a specified angle.
     * @param angle The angle to move in degrees.
     * @param direction The direction to move (0 for closed, 1 for open).
     */
    void moveAngle(float angle, int direction);

    /**
     * @brief Moves the stepper motor asynchronously a specified number of steps.
     * @param steps The number of steps to move.
     * @param direction The direction to move (0 for closed, 1 for open).
     * @param callback A callback function to call when the movement is complete.
     */
    void moveStepsAsync(int steps, int direction, std::function<void()> callback);

    /**
     * @brief Stops the movement of the stepper motor.
     */
    void stopMovement();

    /**
     * @brief Performs an emergency stop of the stepper motor.
     */
    void emergencyStop();

    /**
     * @brief Calibrates the stepper motor using the limit switches.
     */
    void calibrate();

    /**
     * @brief Moves the stepper motor to a specified percent open position.
     * @param percent The percent open position (0-100).
     * @param callback A callback function to call when the movement is complete.
     */
    void moveToPercentOpen(float percent, std::function<void()> callback);

    /**
     * @brief Moves the stepper motor to the fully open position.
     */
    void moveToFullyOpen();

    /**
     * @brief Moves the stepper motor to the fully closed position.
     */
    void moveToFullyClosed();

    /**
     * @brief Gets the current step count of the stepper motor.
     * @return The current step count.
     */
    int getCurrentStepCount() const;

    /**
     * @brief Gets the full range step count of the stepper motor.
     * @return The full range step count.
     */
    int getFullRangeCount() const;

    /**
     * @brief Gets the percent open position of the stepper motor.
     * @return The percent open position.
     */
    float getPercentOpen() const;

    /**
     * @brief Checks if the stepper motor is currently moving.
     * @return True if the motor is moving, false otherwise.
     */
    bool isMoving() const;

private:
    /**
     * @brief Converts steps to angle in degrees.
     * @param steps The number of steps.
     * @return The angle in degrees.
     */
    float stepsToAngle(int steps);

    // GPIO pin assignments
    int _stepPin;
    int _dirPin;
    int _enablePin;
    int _stepsPerRevolution;
    int _microstepping;
    float _speed;
    float _maxSpeed;
    float _acceleration;
    int _currentStepCount; // Tracks the current step position relative to the starting point
    int _fullRangeCount; // The total range of steps between limit switches
    bool _isMoving; // Indicates if the motor is currently moving
    bool _isCalibrated; // Indicates if the motor has been calibrated

    // GPIO chip and line pointers
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    gpiod_line *limit_switch_bottom;
    gpiod_line *limit_switch_top;

    // Private methods
    std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h
