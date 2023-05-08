#include <Keypad.h>
#include "DFRobot_RGBLCD1602.h"

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {39, 41, 43, 45}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {47, 49, 51, 53}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

DFRobot_RGBLCD1602 lcd(/*RGBAddr*/0x60 ,/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show

void setup(){
  Serial.begin(9600);

  lcd.init();
}

void loop(){
  char key = keypad.getKey();

  if (key){
    Serial.println(key);

    lcd.clear();

    lcd.print(key);
  }

}
