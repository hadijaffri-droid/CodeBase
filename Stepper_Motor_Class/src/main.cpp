#include <Arduino.h>
#include "StepperMotor.h"
/* MOTOR CONFIGURATION PARATMETERS*/

const int motor_step_pin = 3;
const int motor_direction_pin = 4;
const int motor_enable_pin = 5;
const long motor_max_speed_steps_per_second = 3000;
const long motor_speed_steps_per_second = 2000;
const long motor_acceleration_steps_per_second_squared = 2000;
const long motor_steps_per_rotation = 10000;

/*Create Motor Object*/

Stepper_Motor _Stepper(motor_step_pin,motor_direction_pin,motor_enable_pin);

void setup() 
{
   Serial.begin(9600);
  _Stepper.initiliase_stepper();
  _Stepper.stepper_motor_enable();
  _Stepper.set_motor_max_speed_steps_per_second(motor_max_speed_steps_per_second);
  _Stepper.set_motor_speed_steps_per_second(motor_speed_steps_per_second);
  _Stepper.set_motor_acceleration_steps_per_second_squared(motor_acceleration_steps_per_second_squared);
  _Stepper.set_motor_steps_per_rotation(motor_steps_per_rotation);
}

void loop() 
{
  // put your main code here, to run repeatedly:
}