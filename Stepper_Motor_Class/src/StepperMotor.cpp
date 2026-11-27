#include "StepperMotor.h"
Stepper_Motor::Stepper_Motor(const char* name,uint8_t step_pin,uint8_t direction_pin,uint8_t enable_pin):ShieldBase(name),motor_step_pin(step_pin),motor_direction_pin(direction_pin),motor_enable_pin(enable_pin),Stepper(1,step_pin,direction_pin),used_pins_arr{step_pin,direction_pin,enable_pin}
{
    Stepper.setCurrentPosition(0); // start with current position at 0 when creating the stepper object
}

const char* Stepper_Motor::getName () const
{
    return ShieldBase::getName();
}

void Stepper_Motor::setName(const char* newName)
{
    ShieldBase::setName(newName);
}

const uint8_t* Stepper_Motor::getUsedPins() const
{
    return used_pins_arr;
}

uint8_t Stepper_Motor::getUsedPinsCount () const
{
    return 3;
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
    return round((get_steps_per_rotation()*degrees) / 360.0);
    // round returns the next integer if the fractional part is greater or equal to 0.5 
    // returns the previous integer if the fractional part is less than 0.5
}

void Stepper_Motor::set_max_speed_steps_per_second(long max_speed)
{
    Stepper.setMaxSpeed(max_speed);
}

void Stepper_Motor::set_speed_steps_per_second(long speed)
{
    Stepper.setSpeed(speed);
}

void Stepper_Motor::set_acceleration_steps_per_second_squared(long acceleration )
{
    Stepper.setAcceleration(acceleration);
}

void Stepper_Motor::set_steps_per_rotation(long steps_per_rotation)
{
    motor_steps_per_rotation = steps_per_rotation;
}

long Stepper_Motor::get_max_speed() 
{
    return Stepper.maxSpeed();
}

long Stepper_Motor::get_speed()
{
    return Stepper.speed();
}

long Stepper_Motor::get_acceleration() 
{
    return Stepper.acceleration();
}

long Stepper_Motor::get_steps_per_rotation () const
{
    return motor_steps_per_rotation;
}


bool Stepper_Motor::is_in_target_position()
{
    return Stepper.currentPosition () == Stepper.targetPosition();
}

void Stepper_Motor:: run()
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
