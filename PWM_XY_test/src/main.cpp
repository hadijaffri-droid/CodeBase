#include <Arduino.h>
#include <string.h>
#include "PWM_XY_Shield_cpp.h"
PWM_XY_Shield PWM(Serial1);
void setup() {
  // Channel A: USB Communication with my computer
  Serial.begin(9600);
  delay(2000);
  Serial1.begin(9600);
  PWM.setDutyCycle(17);
  PWM.setFrequency(2800);
  PWM.get_frequency();
  PWM.get_dutyCycle();
  PWM.setMode(1);
}
void loop() 
{
}

