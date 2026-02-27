#include <Keypad.h>
#include <Arduino.h>
#include <Servo.h>
#include <GyverButton.h>

#define SERVO_PIN PB9
#define BUTTON_PIN PB5

const byte SERVO_LOCKED_POS   = 180;
const byte SERVO_UNLOCKED_POS = 0;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {PA7, PA6, PA5, PA4};
byte colPins[COLS] = {PA3, PA2, PA1};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String correctCode = "1234";
String inputCode = "";

Servo lock;
GButton button(BUTTON_PIN);

enum DoorState { LOCKED, UNLOCKED_WAIT_RELEASE, UNLOCKED_READY_CLOSE };
DoorState doorState = LOCKED;

void setup() {
  lock.attach(SERVO_PIN);
  lock.write(SERVO_LOCKED_POS);
  button.setDebounce(50);
  button.setTimeout(300);
  button.setClickTimeout(600);
}

void loop() {
  button.tick();
  handleKeypad();

  switch (doorState) {
    case UNLOCKED_WAIT_RELEASE:
      if (!button.isHold()) doorState = UNLOCKED_READY_CLOSE;
      break;
    case UNLOCKED_READY_CLOSE:
      if (button.isHold()) {
        lock.write(SERVO_LOCKED_POS);
        doorState = LOCKED;
      }
      break;
    default:
      break;
  }
}

void handleKeypad() {
  if (doorState != LOCKED) return;
  char key = keypad.getKey();
  if (!key) return;
  if (key == '*') checkCode();
  else if (key == '#') inputCode = "";
  else if (key >= '0' && key <= '9') inputCode += key;
}

void checkCode() {
  if (inputCode == correctCode && doorState == LOCKED) {
    lock.write(SERVO_UNLOCKED_POS);
    doorState = UNLOCKED_WAIT_RELEASE;
  }
  inputCode = "";
}