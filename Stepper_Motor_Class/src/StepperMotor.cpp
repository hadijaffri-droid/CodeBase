#include "StepperMotor.h"
Stepper_Motor::Stepper_Motor(uint8_t step_pin,uint8_t direction_pin,uint8_t enable_pin):motor_step_pin(step_pin),motor_direction_pin(direction_pin),motor_enable_pin(enable_pin),Stepper(1,step_pin,direction_pin)
{
}

void Stepper_Motor::initiliase_stepper() const
{
    pinMode(motor_enable_pin,OUTPUT);
}

void Stepper_Motor::stepper_motor_enable() const
{
  digitalWrite(motor_enable_pin,HIGH);
}

void Stepper_Motor::stepper_motor_disable() const
{
    digitalWrite(motor_enable_pin,LOW);
}

long Stepper_Motor::degree_to_steps(int degrees)
{
    return (get_motor_steps_per_rotation()*degrees) / 360;
}

void Stepper_Motor::set_motor_max_speed(long max_speed)
{
    motor_max_speed_steps_per_second = max_speed;
    Stepper.setMaxSpeed(motor_max_speed_steps_per_second);
}

void Stepper_Motor::set_motor_speed(long speed)
{
    motor_speed_steps_per_second = speed;
    Stepper.setSpeed(motor_speed_steps_per_second);
}

void Stepper_Motor::set_motor_acceleration(long acceleration)
{
    motor_acceleration_steps_per_second_squared = acceleration;
    Stepper.setAcceleration(motor_acceleration_steps_per_second_squared);
}

void Stepper_Motor::set_motor_steps_per_rotation(long steps_per_rotation)
{
    motor_steps_per_rotation = steps_per_rotation;
}

long Stepper_Motor::get_motor_max_speed() const
{
    return motor_max_speed_steps_per_second;
}

long Stepper_Motor::get_motor_speed() const
{
    return motor_speed_steps_per_second;
}

long Stepper_Motor::get_motor_acceleration() const
{
    return motor_acceleration_steps_per_second_squared;
}

long Stepper_Motor::get_motor_steps_per_rotation () const
{
    return motor_steps_per_rotation;
}

void Stepper_Motor::move_steps_relative(long steps_to_move, int direction)
{
    Stepper.move(direction*steps_to_move);
    while (Stepper.distanceToGo()!= 0)
    {
        Stepper.run();
    }
}

void Stepper_Motor::move_steps_absolute(long steps_to_move,int direction)
{
    Stepper.moveTo(direction*steps_to_move);
    while(Stepper.distanceToGo()!=0)
    {
        Stepper.run();
    }
}
void Stepper_Motor::oscillate_relative(int repititions,long distance_in_angle, int oscillation_start_direction)
{
    long angle_in_steps  = degree_to_steps(distance_in_angle);
    for (int i = 0; i < repititions;i++)
{
  move_steps_relative(angle_in_steps,oscillation_start_direction); 
  move_steps_relative(angle_in_steps,-oscillation_start_direction);
}

}
void Stepper_Motor::oscillate_absolute(int repititions,long distance_in_angle, int oscillation_start_direction)
{
    long angle_in_steps = degree_to_steps(distance_in_angle);
    for (int i = 0; i< repititions; i++)
    {
        move_steps_absolute(angle_in_steps,oscillation_start_direction);
        move_steps_absolute(angle_in_steps,-oscillation_start_direction);
    }
}
// figure out how to use your class object with AccelStepper Object
