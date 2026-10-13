#ifndef DC_MOTOR_H
#define DC_MOTOR_H
#include "PWM_XY_Shield_cpp.h"
#include "Shield_Base.h"
class DC_MOTOR : public ShieldBase
{
    private:
    const uint8_t usedPinsArr[3];
    PWM_XY_Shield pwm;
    public:
    DC_MOTOR(Stream& serial,const char* pwm_name,const char* name,uint8_t pin, uint32_t Hz);
    void spin_clockwise(uint32_t hz,uint8_t percentage);
    void spin_counterclockwise(uint32_t hz,uint8_t percentage);
    const uint8_t* getUsedPins() const override  { return usedPinsArr; }
    uint8_t getUsedPinsCount() const override {return 3;}
    uint8_t pinGet() const;
    void stop();
    void init();
    void set_Motor_frequency(uint32_t Hz);
    void set_Motor_duty(uint8_t percentage);
};
#endif