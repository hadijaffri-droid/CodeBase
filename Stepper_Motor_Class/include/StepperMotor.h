#ifndef STEPPER_MOTR_H
#define STEPPER_MOTOR_H
#include <Arduino.h>
#include <AccelStepper.h>
class Stepper_Motor 
{
 public:
 explicit Stepper_Motor(uint8_t step_pin,uint8_t direction_pin, uint8_t enable_pin);
 long get_motor_speed() const;
 long get_motor_max_speed() const;
 long get_motor_acceleration() const;
 long get_motor_steps_per_rotation() const;
 void set_motor_speed_steps_per_second(long speed = 100);
 void set_motor_max_speed_steps_per_second(long max_speed = 100);
 void set_motor_acceleration_steps_per_second_squared(long acceleration = 50);
 void set_motor_steps_per_rotation(long steps_per_rotation = 1000);
 void initiliase_stepper() const;
 void stepper_motor_enable() const;
 void stepper_motor_disable() const;
 long degree_to_steps(int degrees);
 void move_steps_relative(long steps_to_move,int direction);
 void move_steps_absolute(long steps_to_move,int direction);
 void oscillate_relative(int repititions,long distance_in_angle, int oscillation_start_direction);
void  oscillate_absolute(int repititions,long distance_in_angle, int oscillation_start_direction);
 private:
 long motor_max_speed_steps_per_second;
 long motor_speed_steps_per_second;
 long motor_acceleration_steps_per_second_squared;
 long motor_steps_per_rotation;
 uint8_t motor_enable_pin;
 uint8_t motor_step_pin;
 uint8_t motor_direction_pin;
 AccelStepper Stepper;

};
#endif