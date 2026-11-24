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
  _Stepper.motor_enable();
  _Stepper.set_motor_max_speed_steps_per_second(motor_max_speed_steps_per_second);
  _Stepper.set_motor_speed_steps_per_second(motor_speed_steps_per_second);
  _Stepper.set_motor_acceleration_steps_per_second_squared(motor_acceleration_steps_per_second_squared);
  _Stepper.set_motor_steps_per_rotation(motor_steps_per_rotation);
}

void wait_until_done()
{
  while (!_Stepper.is_in_target_position())
  {
    _Stepper.run_stepper();
  }
}

void loop()
{
  Serial.println("1. One full revolution clockwise");
  _Stepper.move_relative(motor_steps_per_rotation, Stepper_Motor::clockwise);
  wait_until_done();
  delay(1000);

  Serial.println("2. One full revolution counter-clockwise");
  _Stepper.move_relative(motor_steps_per_rotation, Stepper_Motor::counter_clockwise);
  wait_until_done();
  delay(1000);

  Serial.println("3. Quarter turn clockwise (90 degrees)");
  _Stepper.move_angle_relative(90,Stepper_Motor::clockwise);
  wait_until_done();
  delay(1000);

  Serial.println("4. Go to absolute position 0 (home)");
  _Stepper.move_absolute(0, Stepper_Motor::counter_clockwise);
  wait_until_done();
  delay(1000);

  Serial.print("Current position: ");
  Serial.println(_Stepper.get_current_position());
  delay(2000);
}