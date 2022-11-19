#include <Arduino.h>

unsigned long lastTime = millis();
unsigned long interval = 1000;

void digitalToggle(byte pin);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (millis() >= lastTime + interval) {
    digitalToggle(LED_BUILTIN);
    lastTime = millis();    
  }
}

void digitalToggle(uint8_t pin) {
  digitalWrite(pin, !digitalRead(pin));
}