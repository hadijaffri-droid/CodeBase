#include <Arduino.h>
#include <string.h>
#include "PWM_XY_Shield_cpp.h"
PWM_XY_Shield PWM(Serial1);
void setup() {
  Serial.begin(115200);
  while(!Serial)
  {
    delay(100);
  }
  Serial.println("Starting routine!!");
  Serial1.begin(9600);
  PWM.setFrequency(56900);
  PWM.setDutyCycle(65);
}

void loop() 
{
}
/*
For the XY LPWM module read in lower case returns frequency and duty information
XY LPWM returns frequency information appended with Hz.
read is unreliable. Does not return the information correctly all the time
The duty cylce is set with Captial D in both the modules.
*/
