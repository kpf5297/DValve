
#include "StepperMotor.h"

// Constructor: Initializes the GPIO pins for the enable, pulse, and direction lines
StepperMotor::StepperMotor(int enablePin, int pulsePin, int directionPin, const std::string& chipName) {
    chip = gpiod_chip_open_by_name(chipName.c_str()); // Open the GPIO chip
    initializeGPIO(enablePin, &enableLine); // Initialize enable line
    initializeGPIO(pulsePin, &pulseLine); // Initialize pulse line
    initializeGPIO(directionPin, &directionLine); // Initialize direction line
    setSpeed(60); // Set default speed to 60 RPM
    setAcceleration(100); // Set default acceleration to 100 RPM/s
    setMicrostepping(1); // Set default microstepping to 1 (no microstepping)
}

// Destructor: Releases the GPIO chip
StepperMotor::~StepperMotor() {
    gpiod_chip_close(chip); // Close the GPIO chip
}

// Initializes a GPIO line for output
void StepperMotor::initializeGPIO(int pin, gpiod_line** line) {
    *line = gpiod_chip_get_line(chip, pin); // Get the line
    gpiod_line_request_output(*line, "stepper_motor", 0); // Request line for output with initial value low
}

// Sets the direction of motor rotation
void StepperMotor::setDirection(bool direction) {
    gpiod_line_set_value(directionLine, direction ? 0 : 1); // Set the direction line value
}

// Enables or disables the motor
void StepperMotor::enableMotor(bool enable) {
    gpiod_line_set_value(enableLine, enable ? 0 : 1); // Set the enable line value
}

// Sets the motor speed in RPM
void StepperMotor::setSpeed(long rpm) {
    // Convert RPM to microseconds delay between steps
    long stepsPerRevolution = 200 * microstepping; // Assuming 200 full steps per revolution
    stepDelay = 60L * 1000L * 1000L / (stepsPerRevolution * rpm);
}

// Sets the motor acceleration (RPM/s)
void StepperMotor::setAcceleration(long rpm_per_second) {
    acceleration = rpm_per_second; // Set acceleration value
    // Dynamic acceleration control logic would be implemented here
}

// Sets the microstepping mode (up to x16)
void StepperMotor::setMicrostepping(int microsteps) {
    microstepping = microsteps; // Set microstepping value
    // Additional logic for microstepping configuration goes here
}

// Moves the motor a specific number of steps
void StepperMotor::move(int steps) {
    bool direction = steps > 0;
    setDirection(direction); // Set the direction based on steps sign
    enableMotor(true); // Enable the motor

    int stepsToMove = abs(steps);
    for(int i = 0; i < stepsToMove; ++i) {
        step(); // Make a single step
        std::this_thread::sleep_for(calculateStepDelay(stepsToMove - i)); // Wait for the next step
    }

    enableMotor(false); // Disable the motor after movement
}

// Performs a single step
void StepperMotor::step() {
    gpiod_line_set_value(pulseLine, 1); // Set pulse line high
    std::this_thread::sleep_for(std::chrono::microseconds(1)); // Wait for pulse width
    gpiod_line_set_value(pulseLine, 0); // Set pulse line low
}

// Calculates the delay needed for each step, considering acceleration
std::chrono::microseconds StepperMotor::calculateStepDelay(int stepsRemaining) {
    // Implementation of dynamic step delay calculation would go here
    return std::chrono::microseconds(stepDelay); // Placeholder implementation
}
