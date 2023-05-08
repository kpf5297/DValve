/*********************************************************************************************
    Driven DValve 
  
    Control is run by using a serial emulator to command movement

    1 = 1 step without microstepping

    A positive integer will open the valve. 

    A negative integer will close the valve.
*********************************************************************************************/
#include <AccelStepper.h>

#define DIR_PIN 6             // Direction pin
#define STEP_PIN 7            // Step pin
#define TOP_LIMIT 2           // Interrupt pin for top limit
#define BOTTOM_LIMIT 3        // Interrupt pin for bottom limit

#define STEPS_PER_REV 200     // Number of steps per revolution
#define SPEED 200             // Motor speed in steps per second
#define ACCEL 50              // Motor acceleration in steps per second per second

#define TOP_LIMIT_STATE 0     // The state of the top limit pin when the limit is reached
#define BOTTOM_LIMIT_STATE 1  // The state of the bottom limit pin when the limit is reached

#define REVERSE_STEPS 1000    // The number of steps to reverse when a limit is reached

bool setDirection = false;    // false = closing valve, HIGH = opening valve
bool motorRunning = false;

void reverseMotor() {
  setDirection = !setDirection;
}

bool motorState() {
  return motorRunning;
}

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

  Serial.begin(9600); // Start serial communication

    while (!Serial) {
      ;
  }

  // pinMode(TOP_LIMIT, INPUT_PULLUP);
  // pinMode(BOTTOM_LIMIT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOP_LIMIT), top_limit_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(BOTTOM_LIMIT), bottom_limit_isr, FALLING);
  stepper.setMaxSpeed(SPEED);
  stepper.setAcceleration(ACCEL);
}

void loop() {

  Serial.print("Enter a number of steps: ");

  while(!Serial.available()) {
    ;
  }
  
    int GOTO = Serial.parseInt();
    Serial.print("You entered: ");
    Serial.println(GOTO);

    if (GOTO != 0) {
    stepper.move(GOTO);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
    }


  
  
  
  
  
  if (top_limit_triggered) {
    // Serial.println("Top Limit triggered.");
    // stepper.stop();
    // stepper.move(-REVERSE_STEPS);
    // while (stepper.distanceToGo() != 0) {
    //   stepper.run();
    // }
    top_limit_triggered = false;
  }
  
  if (bottom_limit_triggered) {
    // Serial.println("Bottom Limit triggered.");
    // stepper.stop();
    // stepper.move(REVERSE_STEPS);
    // while (stepper.distanceToGo() != 0) {
    //   stepper.run();
    // }
    bottom_limit_triggered = false;
  }
  
  stepper.run();
}
