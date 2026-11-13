#include <Arduino.h>
#include "StepperMotor.h"
Stepper_Motor _Stepper(3,4,5); // assign dummy pins for now
void setup() 
{
   Serial.begin(9600);
  _Stepper.initiliase_stepper();
  _Stepper.stepper_motor_enable();
  _Stepper.set_motor_max_speed(3000);
  _Stepper.set_motor_speed(2000);
  _Stepper.set_motor_acceleration(2000);
  _Stepper.set_motor_steps_per_rotation(10000);
}

void loop() 
{
  // put your main code here, to run repeatedly:
}