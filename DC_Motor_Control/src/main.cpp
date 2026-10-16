#include <Arduino.h>
#include "DC_Motor.h"
// create a DC motor object
DC_MOTOR Motor(Serial1,"DC_MOTOR","PWM_SHIELD_XY",10,1200);
void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Motor.init();
  Motor.spin_clockwise(8800,60); // specify the frequency and the duty cycle
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
