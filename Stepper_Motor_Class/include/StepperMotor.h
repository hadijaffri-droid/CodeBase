#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <Arduino.h>
#include <AccelStepper.h>
#include <bits/stdc++.h>

class Stepper_Motor 
{
 public:
 explicit Stepper_Motor(uint8_t step_pin,uint8_t direction_pin, uint8_t enable_pin);
 enum Direction 
 {
    clockwise,
    counter_clockwise
 };
 long get_motor_speed();
 long get_motor_max_speed();
 long get_motor_acceleration();
 long get_motor_steps_per_rotation() const;
 void set_motor_speed_steps_per_second(long speed = 100);
 void set_motor_max_speed_steps_per_second(long max_speed = 100);
 void set_motor_acceleration_steps_per_second_squared(long acceleration = 50);
 void set_motor_steps_per_rotation(long steps_per_rotation = 1000);
 void move_relative(long relative_steps_to_move,Direction direction); // only sets a relative target Position, must be used alongside run stepper to run the motor
 void move_absolute(long position,Direction direction); // only sets an absolute target Position, must be sued alongside run stepper to run the motor
 void initiliase_stepper() const;
 void motor_enable() const;
 void motor_disable() const;
 long degree_to_steps(int degrees);
 bool is_in_target_position();
 void run_stepper();
 void move_angle_relative(int degrees);
 void move_angle_absolute(int degrees);
 long get_current_position();
 void set_position(long position);
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