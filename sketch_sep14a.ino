/* This is a firmware to control filter wheel;
 *  20220914
 *  First implementation
 *  
 *  20220915
 *  Add features to only enable the motor while moving is needed to prevent the driver and motor overheat. 
 *  
 *  20220916
 *  Push the rotating speed of the motor by try and error the SetMaxSpeed and SetAcceleration parameters
 *  
 *  20220918 Update
 *  Add variables: stoptime, holdtime and stopflag to solve the over shoot problem. In last version, the motor 
 *  will be disabled once it stops moving. This also remove the holding torque of the motor. As the filter wheel
 *  rotates at very high speed ( 60 degrees/ 45 ms), the filter wheel will over shoot ( go more than 60 degrees)
 *  and the over shoot can accumulate. To solve this problem, the toqure was hold for a short period of time 
 *  (holdtime) to prevent the overshoot. 
 *  
 *  Todo
 *  Initial position setup/reference
 */
#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER,2,3);
int i = 1;
// constants won't change. They're used here to set pin numbers:
const int y_buttonPin = 6;    // the number of the yellow pushbutton pin
const int g_buttonPin = 7;
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

int g_buttonState;
int last_g_ButtonState = LOW;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long last_g_DebounceTime=0;
unsigned long debounceDelay = 60;    // the debounce time; increase if the output flickers
unsigned long stoptime;

int holdtime = 100;
bool stopflag = 0;
void setup() {
  
  pinMode(y_buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

  stepper.setEnablePin(4);
  stepper.setPinsInverted(false, false, true);
  stepper.setMaxSpeed(240000.0); //600000
  stepper.setAcceleration(120000.0);

  Serial.begin(9600);
}

void loop() {
  stepper.runToNewPosition(66.66*i);
  if (stopflag && !stepper.isRunning()){
      stoptime = millis(); 
      stopflag = 0;
  }
  if (millis() - stoptime > holdtime)
      stepper.disableOutputs();
  
  // read the state of the switch into a local variable:
  int y_reading = digitalRead(y_buttonPin);
  int g_reading = digitalRead(g_buttonPin);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (y_reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (y_reading != buttonState) {
      buttonState = y_reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        stepper.enableOutputs();
        stopflag = 1;

        i++;
        Serial.print("i = ");Serial.println(i);
      }
    }
  }


  if (g_reading != last_g_ButtonState) {
    // reset the debouncing timer
    last_g_DebounceTime = millis();
  }

  if ((millis() - last_g_DebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (g_reading != g_buttonState) {
      g_buttonState = g_reading;

      // only toggle the LED if the new button state is HIGH
      if (g_buttonState == HIGH) {
        ledState = !ledState;
        stepper.enableOutputs();
        stopflag = 1;

        i--;
        Serial.print("i = ");Serial.println(i);
      }
    }
  }


  // set the LED:
  digitalWrite(ledPin, ledState);

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = y_reading;
  last_g_ButtonState = g_reading;
}
