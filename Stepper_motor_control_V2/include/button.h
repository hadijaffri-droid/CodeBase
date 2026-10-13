// LED.h
#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h> 
#include <stddef.h>
#include "Shield_Base.h"


class Button : public ShieldBase{
	private:
	uint8_t pin;
	int debounceDelay;
    bool lastReading;
    bool isActiveVar;
    unsigned long lastChangeTime;
	uint8_t usedPinsArr[1];
	const char* buttonName;
	static Button* instance;
	volatile bool pressed;
		// bool pinIndexValid;
		// bool checkPinIndexValid();
	public:
		Button(const char* name, uint8_t buttonPin, int debounceDelay = 50);
		// ~Button();
		const char* getName() const override;
    const uint8_t* getUsedPins() const override  { return usedPinsArr; }
    uint8_t getUsedPinsCount() const override {return 1;}
		uint8_t pinGet();
		void debounceDelaySet(int delayIn_ms);
		int debounceDelayGet();
    bool isActive();
    bool isActive(bool updateState);
		bool buttonChangedState();
    bool buttonWasPressed();
    bool buttonWasUnpressed();
	void begin(bool useInterrupt); // this function is used to assing this to instance pointer
	static void interruptWrapper(); // attach interrupt command expects a pointer to a static function
	void handleInterrupt();
	bool get_pause_state();
};
#endif

