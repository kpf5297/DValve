/****************************************************************************************

  DValve Numpad control

    Numbers 0 - 9 will move the motor n * xxxx steps
    * (splat) will reverse the direction of the motor
    # (pound) will start and stop the motor

****************************************************************************************/

#include <AccelStepper.h>
#include <Keypad.h>

#define DIR_PIN 2     // Direction pin
#define STEP_PIN 3    // Step pin
#define TOP_LIMIT 4   // Interrupt pin for top limit
#define BOTTOM_LIMIT 5 // Interrupt pin for bottom limit

#define STEPS_PER_REV 200   // Number of steps per revolution
#define SPEED 1000    // Motor speed in steps per second
#define ACCEL 5000   // Motor acceleration in steps per second per second

#define TOP_LIMIT_STATE 0 // The state of the top limit pin when the limit is reached
#define BOTTOM_LIMIT_STATE 1 // The state of the bottom limit pin when the limit is reached
#define REVERSE_STEPS 1000 // The number of steps to reverse when a limit is reached

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {6, 7, 8, 9}; // Row pins
byte colPins[COLS] = {10, 11, 12, 13}; // Column pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
volatile boolean top_limit_triggered = false;
volatile boolean bottom_limit_triggered = false;
boolean reverse_direction = false;
boolean motor_running = false;

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
  Serial.begin(9600);
}

void loop() {
  char key = keypad.getKey();
  if (key != NO_KEY) {
    if (key >= '0' && key <= '9') {
      int steps = key - '0';
      if (reverse_direction) {
        steps = -steps;
      }
      stepper.move(steps * STEPS_PER_REV);
      motor_running = true;
    } else if (key == '*') {
      reverse_direction = !reverse_direction;
    } else if (key == '#') {
      if (motor_running) {
        stepper.stop();
        while (stepper.distanceToGo() != 0) {
          stepper.run();
        }
        motor_running = false;
      } else {
        stepper.run();
        motor_running = true;
      }
    }
  }
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

