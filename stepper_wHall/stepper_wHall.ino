#include <AccelStepper.h>

#define DIR_PIN 6             // Direction pin
#define STEP_PIN 7            // Step pin
#define TOP_LIMIT 2           // Interrupt pin for top limit
#define BOTTOM_LIMIT 3        // Interrupt pin for bottom limit

#define STEPS_PER_REV 200     // Number of steps per revolution
#define SPEED 50            // Motor speed in steps per second
#define ACCEL 20            // Motor acceleration in steps per second per second

#define TOP_LIMIT_STATE 0     // The state of the top limit pin when the limit is reached
#define BOTTOM_LIMIT_STATE 1  // The state of the bottom limit pin when the limit is reached

#define REVERSE_STEPS 1000    // The number of steps to reverse when a limit is reached


AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

volatile boolean top_limit_triggered = false;
volatile boolean bottom_limit_triggered = false;

void top_limit_isr() {
  top_limit_triggered = true;
}

void bottom_limit_isr() {
  bottom_limit_triggered = true;
}

void setup() {
  pinMode(TOP_LIMIT, INPUT_PULLUP);
  pinMode(BOTTOM_LIMIT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOP_LIMIT), top_limit_isr, TOP_LIMIT_STATE);
  attachInterrupt(digitalPinToInterrupt(BOTTOM_LIMIT), bottom_limit_isr, BOTTOM_LIMIT_STATE);
  stepper.setMaxSpeed(SPEED);
  stepper.setAcceleration(ACCEL);
}

void loop() {
  
  if (top_limit_triggered) {
    top_limit_triggered = false;
    stepper.stop();
    stepper.move(-REVERSE_STEPS);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
  
  if (bottom_limit_triggered) {
    bottom_limit_triggered = false;
    stepper.stop();
    stepper.move(REVERSE_STEPS);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
  
  stepper.run();
}
