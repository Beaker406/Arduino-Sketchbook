/*
  Blink_Improved

  Turns an LED on and off for a defined period repeatedly without blocking
  other code from running.

  Created Nov 2022
  by Beaker406

  This sketch is intended to be an improved, non-blocking version of the Blink
  example provided by Arduino which can be found here:
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink

  Arduino has a similar sketch named Blink without Delay:
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/BlinkWithoutDelay

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  This example code is in the public domain.

  VS Code Extension Code Spell Checker ignore list
  cSpell:ignore Arduinos, millis
*/

#include <Arduino.h> // comment this line out if using the Arduino IDE

// set the interval between LED state changes in milliseconds here
const unsigned long runInterval = 1000UL;

// set the pin number the LED is connected to, change if LED is user supplied
const int ledPin = LED_BUILTIN;

unsigned long lastMillis = millis(); // last LED toggle time
unsigned long currentMillis; // current runtime set each loop iteration

void digitalToggle(uint8_t pin); // digitalToggle function prototype

void setup() {
  pinMode(ledPin, OUTPUT); // set the digital pin as an output
  digitalWrite(ledPin, HIGH); // turn the LED on initially
}

void loop() {
  currentMillis = millis(); // record the current runtime of the loop

  /*
    if enough time has past such that:
    current runtime minus the last toggle time is greater than the run interval
    then toggle the LED and record the last toggle time
  */
  if (currentMillis - lastMillis >= runInterval) {
    digitalToggle(ledPin);
    lastMillis = millis();
  }
}

void digitalToggle(uint8_t pin) {
  // read the current state of the pin and write the inverse
  digitalWrite(pin, !digitalRead(pin));
}