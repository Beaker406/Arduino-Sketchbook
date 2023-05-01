/*
  Combination Lock

  This sketch controls an accessory circuit attached to a digital pin using a combination of push-
  buttons. To action the accessory circuit, the user must press the priming button followed by the
  correct combination of combo buttons. If the user inputs the wrong combo, they can continue to
  input combos until a timeout is reached. Afterwards, they must press the priming button again
  before making another attempt.

  Primary system states:
  - Not Primed.......The blue LED is illuminated to indicate the system is ready to be primed.
                     The system is not listening for any combo button presses or logging them.
  - Primed...........The blue LED will turn off and the system is now listening for combo buttons.
                     If a full length combo is entered, the primed state will check if it's correct.
                     If the timeout is reached, the system will return to an unprimed state.
  - Correct Combo....Turn on the green LED and the accessory circuit.
                     If the system was flashing red from a previous incorrect input, stop flashing.
  - Incorrect Combo..Tell the system to begin flashing red on next loop.
                     Re-prime the system for additional attemps within the timeout window.

  In addition to the primary system states, the system is always listening for the priming button
  and managing the state of the red LED. From any state, if the priming button is pressed the input
  timer is reset, the combo input index is reset to zero, and the system is set to the primed state
  to allow new combo attemps. From the correct combo state, the priming button also acts as a reset
  button to turn off the accessory circuit. Note: after reseting the system, it is immediately
  prepared to accept new combo input until the timeout is reached. If you want to turn off the
  accessory and leave it off, press the priming button and allow the timeout to pass. The system
  will enter the not primed state and the blue LED will illuminate.

  The flashing of the red LED is handled outside of the incorrect combo state so as not to
  interrupt new combo attempts. Regardless of the red flashing alerting the user to an incorrect
  combo attempt, they may immediately try a new combo and if they get it right, the red flashing
  will be halted as the green LED and accessory are turned on.

  User defined settings in the global scope:
  - digital pin for the accessory circuit
  - digital pins for the green, red, and blue LEDs
  - digital pin for the priming button
  - debounce time for the priming button
  - digital pins for the combo buttons
  - debounce time for the combo buttons
  - lock combo
  - combo input timeout in milliseconds
  - red flash characteristics after an incorrect combo is entered

  The circuit:
  - an accessory circuit, such as a relay, connected from pin 4 to ground
  - greed LED attached from pin 5 to ground through 220 ohm resistor
  - red LED attached from pin 6 to ground through 220 ohm resistor
  - blue LED attached from pin 7 to ground through 220 ohm resistor
  - priming pushbutton attached from pin 8 to +5V
  - 10kΩ pull-down resistor attached from pin 8 to ground
  - combo pushbuttons x3 attached separately from pins 9, 10, and 11 to +5V
  - 10kΩ pull-down resistors attached separately from pins 9, 10, and 11 to ground

  Pull down switch references:
  - circuit: https://docs.arduino.cc/static/73702ee121860fa04c7f6db5bc77183b/29114/circuit.png
  - schematic: https://docs.arduino.cc/static/8bd1ab1a2a8d384f739bdddcc5bca703/29114/schematic.png

  Note: you may deviate from the use of pull-down resistors. You can optionally use external pull-
  up resistors with the standard input mode or use no external resistors and the INPUT_PULLUP mode.
  Review the button library for proper initialization of button objects. Digital pin 13 is harder
  to use as a digital input than the other digital pins because it has an LED and resistor attached
  to it that's soldered to the board on most boards. If you enable its internal 20kΩ pull-up
  resistor, it will hang at around 1.7V instead of the expected 5V because the onboard LED and
  series resistor pull the voltage level down, meaning it always returns LOW. If you must use pin
  13 as a digital input, set its pin mode to INPUT and use an external pull-down resistor.

  Dependencies:
  - button library

  created 27 Nov 2022
  by Beaker406

  MIT License
  Copyright (c) 2022 Beaker406
  https://github.com/Beaker406/Arduino-Sketchbook/blob/main/LICENSE
*/

// #include <Arduino.h>  // comment this line out if using the Arduino IDE
#include <button.h>

// tl;dr only make changes to numerical constants
//       leave any computed or derived variables alone

// set output pin numbers here
const int ACCESSORY_PIN = 4;
const int GREEN_LED_PIN = 5;
const int RED_LED_PIN = 6;
const int BLUE_LED_PIN = 7;

// set primer button pin and debounce time here
const int primingButtonPin = 8;
const unsigned long primingButtonDebounceTime = 50;
button primingButton(primingButtonPin, INPUT, false);

// set combo button pins and their debounce time here (expand past 3 if desired)
const int comboButtonPins[] = {9, 10, 11};
const unsigned long comboButtonsDebounceTime = 50;
const int comboButtonsCount = sizeof(comboButtonPins) / sizeof(int);
button comboButtons[comboButtonsCount] = {};

// set a lock combo of any length here
const int lockCombo[] = {0, 1, 2};
const int lockComboLength = sizeof(lockCombo) / sizeof(int);
int comboInput[lockComboLength] = {};
int comboInputIndex;

// set the amount of time in milliseconds the user has to input the combo
const unsigned long comboInputTimeOut = 10000;
unsigned long comboInputStartMillis;

// set flash red characteristics
// total flashing time = count * interval * 2
const int flashRedCount = 5;         // number of times to flash the red LED
const long toggleRedInterval = 100;  // milliseconds LED is on for and then off for
const int toggleRedCount = flashRedCount * 2;
bool flashRed = false;
int toggleRedIndex;
unsigned long lastToggleRedMillis = millis();

// enumerate possible states and initialize the system
enum systemState { NOT_PRIMED,
                   PRIMED,
                   CORRECT_COMBO,
                   INCORRECT_COMBO };
int currentSystemState = NOT_PRIMED;

// forward declarations of functions
bool isTimedOut(unsigned long startTime, unsigned long timeOut);
bool compareArrays(const int arrayA[], const int arrayB[], const int length);

void setup() {
  pinMode(ACCESSORY_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  primingButton.setDebounceTime(primingButtonDebounceTime);

  // initialize combo buttons within buttons array
  for (int i = 0; i < comboButtonsCount; i++) {
    comboButtons[i] = button(comboButtonPins[i], INPUT, false);
    comboButtons[i].setDebounceTime(comboButtonsDebounceTime);
  }
}  // end setup

void loop() {
  // always monitor the primer button regardless of the current system state
  primingButton.loop();
  if (primingButton.isPressed()) {
    currentSystemState = PRIMED;
    comboInputIndex = 0;
    comboInputStartMillis = millis();
  }

  // after an incorrect combo, flash red without blocking continued execution
  if (flashRed) {
    if (toggleRedIndex < toggleRedCount) {
      if (millis() - lastToggleRedMillis >= toggleRedInterval) {
        digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
        lastToggleRedMillis = millis();
        toggleRedIndex++;
      }
    } else {
      flashRed = false;
      toggleRedIndex = 0;
    }
  }

  // run current system state specific code
  switch (currentSystemState) {
    case NOT_PRIMED: {
      // set pin states and continue to wait for primer button
      digitalWrite(ACCESSORY_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, HIGH);

      break;
    }  // end case NOT_PRIMED

    case PRIMED: {
      // set pin states
      digitalWrite(ACCESSORY_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, LOW);

      // check each combo button for a press
      for (int i = 0; i < comboButtonsCount; i++) {
        comboButtons[i].loop();
        if (comboButtons[i].isPressed()) {
          comboButtons[i].loop();  // loop after pressed ensures hung state is cleared

          comboInput[comboInputIndex] = i;  // store index of pressed button
          comboInputIndex++;                // and increase input index for next pressed button
        }
      }

      // full code entered, evaluate it and set system state for next run
      if (lockComboLength == comboInputIndex) {
        if (compareArrays(lockCombo, comboInput, lockComboLength))
          currentSystemState = CORRECT_COMBO;
        else
          currentSystemState = INCORRECT_COMBO;
      }

      // monitor combo intput timeout status, reset system state after timeout
      if (isTimedOut(comboInputStartMillis, comboInputTimeOut))
        currentSystemState = NOT_PRIMED;

      break;
    }  // end case PRIMED

    case CORRECT_COMBO: {
      // set pin states
      digitalWrite(ACCESSORY_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, LOW);

      // stop flashing red if correct combo was entered after
      // an incorrect attempt but before flashing completed
      flashRed = false;

      break;
    }  // end case CORRECT_COMBO

    case INCORRECT_COMBO: {
      // prepare to flash the red LED
      flashRed = true;
      toggleRedIndex = 0;

      // re-prime the system for additional attempts within the input window
      currentSystemState = PRIMED;
      comboInputIndex = 0;

      break;
    }  // end case INCORRECT_COMBO

    default: {
      // this should never ever execute but exists anyways just in case
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(BLUE_LED_PIN, HIGH);
      delay(10000);
      break;
    }  // end case default
  }    // end switch
}  // end loop

// check if elapsed time from start time to now exceeds a timeout in milliseconds
bool isTimedOut(unsigned long startTime, unsigned long timeOut) {
  if (millis() - startTime >= timeOut)
    return true;
  else
    return false;
}

// not the most elegant solution comparing each element, but suitable for short combos
bool compareArrays(const int arrayA[], const int arrayB[], const int length) {
  bool same = true;
  long i = 0;
  while (i < length && same) {
    same = arrayA[i] == arrayB[i];
    i++;
  }
  return same;
}