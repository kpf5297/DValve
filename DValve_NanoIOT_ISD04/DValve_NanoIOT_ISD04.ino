/*********************************************************************************************
    Driven DValve

    Author: Kevin Fox 
  
    Control is run by using a serial emulator to command movement

    1 = 1 step without microstepping

    A positive integer will open the valve. 

    A negative integer will close the valve.

    Driver ISD04 Maximums: 
      Whole step: 12kHz
      3200 microstep: 200kHz

    Current Operating Speed 
      Measured: 200Hz
*********************************************************************************************/
#include <AccelStepper.h>
#include <Keypad.h>
#include "DFRobot_RGBLCD1602.h"

#define DIR_PIN 4             // Direction pin
#define STEP_PIN 5            // Step pin
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

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {10, 11, 12, 13}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x60 ,/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show

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

  lcd.init();

  // pinMode(TOP_LIMIT, INPUT_PULLUP);
  // pinMode(BOTTOM_LIMIT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOP_LIMIT), top_limit_isr, FALLING);
  attachInterrupt(digitalPinToInterrupt(BOTTOM_LIMIT), bottom_limit_isr, FALLING);
  stepper.setMaxSpeed(SPEED);
  stepper.setAcceleration(ACCEL);
}

void loop() {

  // if(stepper.distanceToGo() == 0) {
  //   Serial.print("Enter a number of steps: ");
  // }

  // Serial.print("Enter a number of steps: ");
  
  while(!Serial.available()) {
    ;
  }
  
  int GOTO = Serial.parseInt();

  if (GOTO != 0) {

    
    Serial.print("\nYou entered: ");
    Serial.print(GOTO);

    if (GOTO > 0) {
      Serial.print("\nNow opening valve");
    }
    
    if (GOTO < 0) {
      Serial.print("\nNow closing valve");
    }
  //  lcd.print("You entered: ");
  //  lcd.print(GOTO);

    stepper.move(GOTO);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }

    if(stepper.distanceToGo() == 0) {
    Serial.print("\nEnter a number of steps: ");
    }
  }

  // if (stepper.distanceToGo() == 0) {
  //   lcd.clear();
  // }

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
  
  // stepper.run();
}
