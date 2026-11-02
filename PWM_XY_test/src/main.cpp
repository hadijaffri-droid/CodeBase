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
  PWM.setMode(1);
  PWM.setDutyCycle(76);
  PWM.setFrequency(3800);
  float f_val = PWM.getFrequency();
  Serial.println(f_val);
}
void loop() 
{
}

