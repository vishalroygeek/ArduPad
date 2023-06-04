#include <Arduino.h>
#include <Keypad.h>
#include <Constants.h>
#include <Bounce2.h>

//Declaring constants and variables
const int elevatedKeyPin = 6;
const int rePushKeyPin = 4;
const int rePinA = 3;
const int rePinB = 2;
unsigned long reLastIncrementReadTime = micros(); 
unsigned long reLastDecrementReadTime = micros(); 
unsigned int rePauseLength = 30000;
int reFastIncrement = 5;
int elevatedKeyState, rePushKeyState;

//Initializing objects
Bounce2::Button elevatedKey = Bounce2::Button();
Bounce2::Button rePushKey = Bounce2::Button();

//Initializing keypad
const byte keypadRowsCount = 3;
const byte keypadColumnsCount = 3;
byte keypadRowPins[keypadRowsCount] = {9,8,7}; 
byte keypadColumnPins[keypadColumnsCount] = {12,11,10}; 
char keypadKeymap[keypadRowsCount][keypadColumnsCount] = 
{{KEY_VALUE_1, KEY_VALUE_4, KEY_VALUE_7},{KEY_VALUE_2, KEY_VALUE_5, KEY_VALUE_8},{KEY_VALUE_3, KEY_VALUE_6, KEY_VALUE_9}};
Keypad keypad = Keypad(makeKeymap(keypadKeymap), keypadRowPins, keypadColumnPins, keypadRowsCount, keypadColumnsCount);

void readEncoder() {
  static int old_AB = 3;
  static int encoderValue = 0;
  static const int enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
  old_AB <<= 2;

  // Adding current state of pin A and B
  if (digitalRead(rePinA)) old_AB |= 0x02;
  if (digitalRead(rePinB)) old_AB |= 0x01;
  
  encoderValue += enc_states[(old_AB & 0x0f)];

  // Update counter if encoder has rotated a full indent, that is at least 4 steps
  if( encoderValue > 3 ) {
    int changeValue = 1;
    if((micros() - reLastIncrementReadTime) < rePauseLength) {
      changeValue = reFastIncrement * changeValue; 
    }
    reLastIncrementReadTime = micros();
    Serial.println(KEY_VALUE_RE+changeValue);
    encoderValue = 0;
  }else if( encoderValue < -3 ) {
    int changeValue = -1;
    if((micros() - reLastDecrementReadTime) < rePauseLength) {
      changeValue = reFastIncrement * changeValue; 
    }
    reLastDecrementReadTime = micros();
    Serial.println(KEY_VALUE_RE+changeValue);
    encoderValue = 0;
  }
} 

void setup(){
  //Initializing pins
  pinMode(rePinA, INPUT_PULLUP);
  pinMode(rePinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rePinA), readEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rePinB), readEncoder, CHANGE);

  //Initializing single click keys
  elevatedKey.attach(elevatedKeyPin,INPUT_PULLUP);
  elevatedKey.interval(5);
  elevatedKey.setPressedState(LOW); 
  rePushKey.attach(rePushKeyPin,INPUT_PULLUP);
  rePushKey.interval(5);
  rePushKey.setPressedState(LOW); 

  Serial.begin(9600);
}
  
void loop(){
  elevatedKey.update();
  rePushKey.update();

  if (elevatedKey.pressed()){
    Serial.println(KEY_VALUE_ELEVATED);
  }

  if (rePushKey.pressed()){
    Serial.println(KEY_VALUE_RE_PUSH);
  }

  if (keypad.getKeys()){
    for (int i=0; i<keypadColumnsCount*keypadRowsCount; i++){
      if (keypad.key[i].stateChanged && keypad.key[i].kstate==PRESSED){
        Serial.println(keypad.key[i].kchar);
      }
    }
  }
}