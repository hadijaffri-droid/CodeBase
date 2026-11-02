#include "DC_Motor.h"

DC_MOTOR::DC_MOTOR(Stream& serial, const char* pwm_name,const char*name, uint8_t pin,uint32_t Hz)
  : ShieldBase(name), usedPinsArr{pin,0,1}, pwm(serial, pwm_name)
{
  set_Motor_frequency(Hz);
}

void DC_MOTOR::set_Motor_frequency (uint32_t Hz)
{
  bool result = false;
  while(!result)
 {
   result = pwm.setFrequency(Hz);
   if(result)
   {
    Serial.println("Duty Cycle set sucessfully!!");
   }
   else
   {
    Serial.println("Could not communicate with the shield, trying again...");
   }
 }
}

void DC_MOTOR::set_Motor_duty(uint8_t percentage)
{
 bool result= false;
 while(!result)
 {
   result = pwm.setDutyCycle(percentage);
   if(result)
   {
    Serial.println("Duty Cycle set sucessfully!!");
   }
   else
   {
    Serial.println("Could not communicate with the shield, trying again...");
   }
 }
}

void DC_MOTOR::init ()
{
  pinMode(pinGet(),OUTPUT);
}

void DC_MOTOR::stop ()
{
  
}

void DC_MOTOR::spin_clockwise(uint32_t hz,uint8_t percentage)
{
  digitalWrite(pinGet(),HIGH);
  set_Motor_duty(percentage);
}

void DC_MOTOR::spin_counterclockwise (uint32_t hz,uint8_t percentage)
{
 digitalWrite(pinGet(),LOW);
 set_Motor_duty(percentage);
}

uint8_t DC_MOTOR::pinGet() const
{
  return usedPinsArr[0]; // first pin is the direction pin
}
