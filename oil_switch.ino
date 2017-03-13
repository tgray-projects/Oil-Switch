/*
 * Detects current via an analog non-invasive current sensor and, when the current is on, turns a servo
 * that will be attached to an oil furnace feed valve.
 * 
 * Uses code from the Open Energy Monitor project:
 * https://github.com/openenergymonitor/EmonLib
 */

#include "EmonLib.h"                   // Include Emon Library
#include <Servo.h>

EnergyMonitor emon1;                   // Create an instance
Servo myservo;                         // create servo object to control a servo

int  pos = 0;           // track servo position
bool ledOn = false;     // track LED status
int  threshold = 1000;  // current on/off threshold

/**
 * Servo on 9, LED builtin, start to the off position.
 */
void setup()
{  
  myservo.attach(9);
  sweep(true);
  pinMode(LED_BUILTIN, OUTPUT);
  emon1.current(1, 111.1);             // Current: input pin, calibration.
}

/**
 * Helper to turn the LED on/off based on current status.
 */
void blinkLed() 
{
  if (ledOn) {
    digitalWrite(LED_BUILTIN, LOW);
    ledOn = false;
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    ledOn = true;
  }
}

/**
 * Moves the servo on or off, and blinks LED fast or slow to indicate direction
 * as a debug/troubleshooting tool.
 * 
 * dir true is off (safely off), dir false is on (fuel is running)
 */
void sweep(bool dir) 
{
  if (dir == true) {
    for (pos = 0; pos <= 180; pos += 1) {
      myservo.write(pos);
      delay(15);
      if (pos %10 == 0) {
        blinkLed();
      }
    }
  } else {
    for (pos = 180; pos >= 0; pos -= 1) {
      myservo.write(pos);
      delay(15);
      if (pos %5 == 0) {
        blinkLed();
      }
    }
  }
  
  // aleays turn off the led when done
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  double Irms = emon1.calcIrms(1480);  // Calculate Irms only

  int power = Irms*230.0;

  if (power > threshold && pos < 0) {
    sweep(true);
  }
  else if (power < threshold && pos > 180) {
    sweep(false);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
