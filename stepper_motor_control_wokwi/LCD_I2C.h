#ifndef LCD_I2C_H
#define LCD_I2C_H

// Install with Arduino Library Manager:
//   hd44780 by Bill Perry
// We use hd44780_I2Cexp because it is reliable on Arduino UNO R4 WiFi.
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <Arduino.h>
#include "Shield_Base.h"


class LCD_I2C : public hd44780_I2Cexp, public ShieldBase  {
	private:
		uint8_t lcd_Addr;
		uint8_t lcd_cols;
		uint8_t lcd_rows;

		static const uint8_t usedPinsArr[2];

	public:
		LCD_I2C(uint8_t lcd_Addr = 0x27, uint8_t lcd_cols = 20, uint8_t lcd_rows = 4);
		~LCD_I2C();
		void write_center(int line, const char *text);
		void init() override;
		const char* getName() const override         {return "I2C LCD Shield";}
    const uint8_t* getUsedPins() const override  { return usedPinsArr; }
    uint8_t getUsedPinsCount() const override { return sizeof(usedPinsArr) / sizeof(usedPinsArr[0]);}
};
#endif  // LCD_I2C.h
