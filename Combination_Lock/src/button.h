/*
  button.h

  This is a modified version of ArduinoGetStarted.com's ezButton library. Major changes include:
  - change class name from ezButton to button
  - add support for any button configuration; by default the library only supports the INPUT_PULLUP
    mode by tracking state changes from HIGH to LOW but changes to the constructors and additional
    logic allow the use of INPUT mode with external pull-up or pull-down resistors
  - change count behavior from tracking rising and falling events to presses and releases for
    easier comprehension of events with separate logic for pull-up and pull-down resistors
  - Doxygen documentation added

  The ezButton documentation is a great place to start to see example use cases of the library:
  https://arduinogetstarted.com/tutorials/arduino-button-library

  To get started with this button library, the following constructors are valid:

  button button1();
  This button is initialized with reference to pin number NOT_A_PIN making it effectively useless
  as is. This constructor is useful when allocating space in an array of buttons. You can set up an
  array of default buttons and reinitialize them later with proper pins.

  button button1(8);
  This button is most similar to its ezButton brother. It is setup for INPUT_PULLUP mode just like
  the exButton class and the pullUpResistor boolean is also set to true automatically.

  button button1(13, INPUT, false);
  This button is initialized as pull-down button and utilizes the extended features of the edits
  made to this library. The second parameter input prepares the button to use an external resistor
  in either the pull-up or pull-down configuration and the third parameter cements this button as a
  pull-down button.

  modified 27 Nov 2022
  by Beaker406

  MIT License
  Copyright (c) 2022 Beaker406
  https://github.com/Beaker406/Arduino-Sketchbook/blob/main/LICENSE

  Original copyright, conditions, and disclaimer of the ezButton library are provided as footnotes.
*/

#ifndef button_h
#define button_h

#include <Arduino.h>
enum countModes { COUNT_PRESSES,
                  COUNT_RELEASES,
                  COUNT_BOTH };

class button {
 private:
  int buttonPin;               // the pin the button is connected to
  unsigned long debounceTime;  // longer times decrease false positives but increase button hold time
  unsigned long count;         // number of events determined by countMode
  int countMode;               // count presses, releases, or both
  bool buttonPullUpResistor;   // is a pull-up resistor in use?

  int previousSteadyState;   // the previous steady state from the input pin, used to detect pressed and released events
  int lastSteadyState;       // the last steady state from the input pin
  int lastFlickerableState;  // the last flickerable state from the input pin

  unsigned long lastDebounceTime;  // the last time the output pin was toggled

  bool isPressed_pullUp(void);
  bool isPressed_pullDown(void);
  bool isReleased_pullUp(void);
  bool isReleased_pullDown(void);

 public:
  button();
  button(int pin);
  button(int pin, int mode, bool pullUpResistor);
  void setDebounceTime(unsigned long time);
  int getState(void);
  int getStateRaw(void);
  bool isPressed(void);
  bool isReleased(void);
  void setCountMode(int mode);
  unsigned long getCount(void);
  void resetCount(void);
  void loop(void);
};

#endif

/*
 * Copyright (c) 2019, ArduinoGetStarted.com. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the ArduinoGetStarted.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARDUINOGETSTARTED.COM "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARDUINOGETSTARTED.COM BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */