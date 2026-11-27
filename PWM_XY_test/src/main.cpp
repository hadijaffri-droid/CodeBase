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

