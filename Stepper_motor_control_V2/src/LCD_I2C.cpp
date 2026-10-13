#include "LCD_I2C.h"

const uint8_t LCD_I2C::usedPinsArr[2] = {A4, A5};

LCD_I2C::LCD_I2C(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows)
:	lcd_Addr(lcd_Addr),
	lcd_cols(lcd_cols),
	lcd_rows(lcd_rows)
{
}

LCD_I2C::~LCD_I2C(){

}

void LCD_I2C::init(){
  // hd44780_I2Cexp auto-detects the I2C backpack.
  // We keep address as a config field for compatibility with the existing API.
  begin(lcd_cols, lcd_rows);
  backlight();
}

void LCD_I2C::write_center(int line, const char *text){
    int len = 0;
    if (text) {
        len = strlen(text);
    }

    int start_col = int((lcd_cols - len)/2);
    if (start_col < 0) start_col = 0;
    if (line < 0) line = 0;
    else if (line > 3) line = 3;

    this->hd44780_I2Cexp::setCursor(start_col, line);
    this->hd44780_I2Cexp::print(text ? text : "");
}


// void LCD_I2C::write_center(int line, const String &text){
// 	int start_col;
// 	start_col = int((lcd_cols - text.length())/2);
// 	if (start_col < 0){
// 		start_col = 0;
// 	}
// 	if (line < 0){
// 		line = 0;
// 	} else if (line > 3) {
// 		line = 3;
// 	}
// 	Serial.print("set cursor: ");
// 	Serial.print(start_col);
// 	Serial.print(" line: ");
// 	Serial.print(line);
// 	this->LiquidCrystal_I2C::setCursor(start_col, line);
// 	this->LiquidCrystal_I2C::print(text);
// 	//delay(1000);
// }