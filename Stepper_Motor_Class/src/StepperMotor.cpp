#include "StepperMotor.h"
Stepper_Motor::Stepper_Motor(uint8_t step_pin,uint8_t direction_pin,uint8_t enable_pin):motor_step_pin(step_pin),motor_direction_pin(direction_pin),motor_enable_pin(enable_pin),Stepper(1,step_pin,direction_pin)
{
    Stepper.setCurrentPosition(0); // start with current position at 0 when creating the stepper object
}

void Stepper_Motor::initiliase_stepper() const
{
    pinMode(motor_enable_pin,OUTPUT);
}

void Stepper_Motor::motor_enable() const
{
  digitalWrite(motor_enable_pin,HIGH);
}

void Stepper_Motor::motor_disable() const
{
    digitalWrite(motor_enable_pin,LOW);
}

long Stepper_Motor::degree_to_steps(int degrees)
{
    return round((get_motor_steps_per_rotation()*degrees) / 360.0);
    // round returns the next integer if the fractional part is greater or equal to 0.5 
    // returns the previous integer if the fractional part is less than 0.5
}

void Stepper_Motor::set_motor_max_speed_steps_per_second(long max_speed)
{
    motor_max_speed_steps_per_second = max_speed;
    Stepper.setMaxSpeed(motor_max_speed_steps_per_second);
}

void Stepper_Motor::set_motor_speed_steps_per_second(long speed)
{
    motor_speed_steps_per_second = speed;
    Stepper.setSpeed(motor_speed_steps_per_second);
}

void Stepper_Motor::set_motor_acceleration_steps_per_second_squared(long acceleration )
{
    motor_acceleration_steps_per_second_squared = acceleration;
    Stepper.setAcceleration(motor_acceleration_steps_per_second_squared);
}

void Stepper_Motor::set_motor_steps_per_rotation(long steps_per_rotation)
{
    motor_steps_per_rotation = steps_per_rotation;
}

long Stepper_Motor::get_motor_max_speed() 
{
    return Stepper.maxSpeed();
}

long Stepper_Motor::get_motor_speed()
{
    return Stepper.speed();
}

long Stepper_Motor::get_motor_acceleration() 
{
    return Stepper.acceleration();
}

long Stepper_Motor::get_motor_steps_per_rotation () const
{
    return motor_steps_per_rotation;
}


bool Stepper_Motor::is_in_target_position()
{
    return Stepper.currentPosition () == Stepper.targetPosition();
}

void Stepper_Motor:: run_stepper()
{
    Stepper.run();
}

void Stepper_Motor:: move_relative(long relative_steps_to_move, Direction direction) 
{
    switch(direction)
    {
        case clockwise:
        Stepper.move(-1*relative_steps_to_move);
        break;
        case counter_clockwise:
        Stepper.move(relative_steps_to_move);
        break;
    }
}

void Stepper_Motor::move_absolute(long position, Direction direction)
{
    switch(direction)
    {
        case clockwise:
        Stepper.moveTo(-1*position);
        break;
        case counter_clockwise:
        Stepper.moveTo(position);
        break;
    }
}

void Stepper_Motor:: move_angle_relative(int degrees, Direction direction)
{
    switch(direction)
    {
        case clockwise:
        Stepper.move(-1*degree_to_steps(degrees));
        break;
        case counter_clockwise:
        Stepper.move(degree_to_steps(degrees));
        break;
    }
}

void Stepper_Motor:: move_angle_absolute(int degrees, Direction direction)
{
    switch(direction)
    {
        case clockwise:
        Stepper.moveTo(-1*degree_to_steps(degrees));
        break;
        case counter_clockwise:
        Stepper.moveTo(degree_to_steps(degrees));
        break;
    }
}

long Stepper_Motor::get_current_position()
{
    return Stepper.currentPosition();
}

void Stepper_Motor::set_position(long position)
{
    Stepper.setCurrentPosition(position);
}