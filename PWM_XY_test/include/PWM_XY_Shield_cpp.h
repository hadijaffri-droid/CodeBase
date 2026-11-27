#ifndef PWM_XY_SHIELD_CPP_H
#define PWM_XY_SHIELD_CPP_H

#include "Shield_Base.h"
#include <Arduino.h>
#include <stdio.h>    // snprintf
#include <stdlib.h>   // strtof, strtoul
#include <string.h>   // strstr, strcmp
#include <ctype.h>    // isdigit

// Driver for the XY-PWM (older) and XY-LPWM serial PWM modules.
// Every function works with both modules unless noted otherwise.
class PWM_XY_Shield : public ShieldBase
{
  public:
    static const uint32_t DEFAULT_MAX_FREQ_HZ = 150000UL;

    explicit PWM_XY_Shield(Stream& serial, const char* name = "pwm shield xy");
    ~PWM_XY_Shield() override = default;

    // ---- ShieldBase interface ----
    const uint8_t* getUsedPins() const override { return usedPinsArr; }
    uint8_t getUsedPinsCount() const override { return 2; }

    // ---- configuration ----
    void setMaxFrequency(uint32_t maxHz)
    {
      _maxFrequency = (maxHz > DEFAULT_MAX_FREQ_HZ) ? DEFAULT_MAX_FREQ_HZ : maxHz;
    }

    // ---- setters ----
    void setName(const char* newName) override;
    bool setFrequency(uint32_t hz);
    bool setDutyCycle(uint8_t percentage);
    bool setMode(int mode);
    void pwm_enable();
    void pwm_disable();

    // ---- getters ----
    float getFrequency();
    uint16_t getDutyCycle();
    const char* getName() const override;
    const uint8_t* getUsedPins() const override;
    // ---- serial helpers ----
    const char* readResponseBuffer();
    void probe(const char* cmd);

  private:
    // hardware
    Stream& _ser;
    const uint8_t usedPinsArr[2];
    uint32_t _maxFrequency;
    float freq_value;

    // serial buffers
    char command_buffer[15];
    char response_buffer[30];
    char duty_buffer[15];
    int response_buffer_count;
    const char* response;

    // shared by setFrequency, getFrequency and getDutyCycle
    static const int MAX_ATTEMPTS = 10; // I chose 10 randomly, during testing it was observed that the max attemps go uptil 3 tries ( to be safe I set it to 10)
    String read_commands[2] = {"read", "READ"};   // [0] = LPWM module, [1] = older module

    // shared by getFrequency and getDutyCycle
    String field_text;               // frequency or duty text found in the reply
    bool field_found;
    int scan_pos;
    int reply_length;
    const char* number_start;
};

#endif
