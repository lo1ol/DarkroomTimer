#pragma once

#include <LiquidCrystal.h>

#include "../Config.h"

class Lcd {
public:
    Lcd() { m_lcd.begin(DISPLAY_COLS, DISPLAY_ROWS); }
    void setCursor(uint8_t c, uint8_t r) { m_lcd.setCursor(c, r); }
    void print(const char* str) { m_lcd.print(str); }

private:
    LiquidCrystal m_lcd{ LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN };
};
