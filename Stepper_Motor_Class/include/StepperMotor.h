#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <Arduino.h>
#include "Shield_Base.h"
#include <AccelStepper.h>
// changes implemented
// Derived the class from Shield Base
// removed the stlib library
// removed "motor" from variable names
// renamed run_stepper to run()
// removed unused variables in setters
class Stepper_Motor : public ShieldBase
{
 public:
 explicit Stepper_Motor(const char *name,uint8_t step_pin,uint8_t direction_pin, uint8_t enable_pin);
 enum Direction 
 {
    clockwise,
    counter_clockwise
 };
 long get_speed();
 long get_max_speed();
 long get_acceleration();
 long get_steps_per_rotation() const;
 void set_speed_steps_per_second(long speed = 100);
 void set_max_speed_steps_per_second(long max_speed = 100);
 void set_acceleration_steps_per_second_squared(long acceleration = 50);
 void set_steps_per_rotation(long steps_per_rotation = 1000);
 void move_relative(long relative_steps_to_move,Direction direction); // only sets a relative target Position, must be used alongside run stepper to run the motor
 void move_absolute(long position,Direction direction); // only sets an absolute target Position, must be sued alongside run stepper to run the motor
 void initiliase_stepper() const;
 void motor_enable() const;
 void motor_disable() const;
 long degree_to_steps(int degrees);
 bool is_in_target_position();
 void run();
 void move_angle_relative(int degrees,Direction direction);
 void move_angle_absolute(int degrees,Direction direction);
 long get_current_position();
 void set_position(long position);
 const char* getName() const override ;
 void setName(const char* newName) override;
 const uint8_t* getUsedPins() const override;
 uint8_t getUsedPinsCount() const override;
 private:
 long motor_steps_per_rotation;
 uint8_t motor_enable_pin;
 uint8_t motor_step_pin;
 uint8_t motor_direction_pin;
 uint8_t used_pins_arr[3];
 AccelStepper Stepper;

};
#endif