#include "Arduino.h"
#include "PWM_XY_Shield_cpp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
PWM_XY_Shield::PWM_XY_Shield(Stream& serial, const char* name) :ShieldBase(name), _ser(serial), usedPinsArr{0,1}, _maxFrequency(DEFAULT_MAX_FREQ_HZ)
{
  response_buffer_count = 0;
}
uint16_t PWM_XY_Shield:: getDutyCycle()
{
  _ser.print("READ");
  response = readResponseBuffer();
  // pointer math below:
  // response contains frequeny (starting with F) and duty_buffer information (staring with D) separated by a null terminator '\0' 
  // response points to the start of the character buffer/array
  // strlen calculates the array size uptil (not including) the first null terminator
  // add + 2 to skip the null terminator and char 'D' and then land on the digits after D
  const char* duty_start = response+strlen(response)+2; 
  strcpy(duty_buffer,duty_start); // copies the char array
  uint16_t duty_value = strtoul(duty_buffer,nullptr,10); // stroul = string to long integer
  Serial.println(duty_value);
  return duty_value;
  return 0;
}
float PWM_XY_Shield::getFrequency()
{
  _ser.print("READ");
  readResponseBuffer(); // purpose is to just empty the response buffer
  return freq_value; // return the value set during setFrequency function call
}
bool PWM_XY_Shield::setFrequency(uint32_t hz)
{
  freq_value = hz;
  snprintf(command_buffer, sizeof(command_buffer), "F%03u", (unsigned)hz);
  _ser.print(command_buffer);
  response=readResponseBuffer();
 if(strcmp(response,"DOWN")== 0)
 {
  return true;
 }
 else
 {
  return false;
 }
}
bool PWM_XY_Shield::setDutyCycle(uint8_t percentage)
{
  snprintf(command_buffer, sizeof(command_buffer), "D%03u",(unsigned)percentage);
 _ser.print(command_buffer);
  response=readResponseBuffer();
 if(strcmp(response,"DOWN")== 0)
 {
  return true;
 }
 else
 {
  return false;
 }
}
const char* PWM_XY_Shield::readResponseBuffer()
{
  response_buffer_count= 0;
  uint32_t start = millis();
  while (millis() - start < 200)        // read for up to 200 ms
  {
    while (_ser.available() > 0)
    {
      char chIn = _ser.read();
      if(response_buffer_count < sizeof(response_buffer)-1)
      {
         response_buffer[response_buffer_count++]= chIn;
      }
    }
  }
  return response_buffer;
}
bool PWM_XY_Shield::setMode(int mode)
{
    if(mode == 1 || mode == 0)
    {
        if(mode==1)
        {
            snprintf(command_buffer, sizeof(command_buffer), "MODE%u", (unsigned)mode);
        }
        else if(mode == 0)
        {
            snprintf(command_buffer,sizeof(command_buffer),"MODE%u",(unsigned)mode);
        }
        return true;
    }
    else
    {
        return false;
    }
}
void PWM_XY_Shield::pwm_enable()
{
  _ser.print("ON");
  response = readResponseBuffer();
}
void PWM_XY_Shield::pwm_disable()
{
  _ser.print("OFF");
  response= readResponseBuffer();
}