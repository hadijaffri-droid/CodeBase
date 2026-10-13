#ifndef PWM_XY_SHIELD_CPP_H
#define PWM_XY_SHIELD_CPP_H
#include <Arduino.h>
#include <string.h>
#include "Shield_Base.h"
class PWM_XY_Shield: public ShieldBase
{
  public:
    static const uint32_t DEFAULT_MAX_FREQ_HZ = 150000UL;
    explicit PWM_XY_Shield(Stream& serial, const char* name = "pwm shield xy");
    ~PWM_XY_Shield() override = default;
    const uint8_t* getUsedPins() const override { return usedPinsArr; }
    uint8_t getUsedPinsCount() const override {return 2;}
    void setMaxFrequency(uint32_t maxHz)
    {
      _maxFrequency = (maxHz > DEFAULT_MAX_FREQ_HZ) ? DEFAULT_MAX_FREQ_HZ : maxHz;
    }
    bool setFrequency(uint32_t hz);
    bool setDutyCycle(uint8_t percentage);
    bool setMode(int mode);
    double get_frequency();
    uint16_t get_dutyCycle();
    char const* read_response();
  private:
    Stream& _ser;
    const uint8_t usedPinsArr[2];
    char command_buffer[15];
    char response_buffer[30];
    char freq[15];
    char duty[15];
    int response_buffer_count;
    uint32_t _maxFrequency;
};
#endif