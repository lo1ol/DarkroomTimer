#include "../Config.h"

#if LCD_VERSION == LCD_VERSION_I2C && !defined(PIO_UNIT_TESTING)

    #include <Wire.h>

    #include "I2C.h"

    #define LCD_ADDR 0x27
    #define LCD_BACKLIGHT 0x08
    #define LCD_ENABLE 0x04
    #define LCD_RS 0x01

namespace {
void lcdSendNibble(uint8_t data, bool isData) {
    data |= LCD_BACKLIGHT;

    if (isData)
        data |= LCD_RS;

    // pulse enable
    // https://github.com/arduino-libraries/LiquidCrystal/blob/8c18c69e9de2fb7d89e64098b1899064fee3ba06/src/LiquidCrystal.cpp#L303
    Wire.beginTransmission(LCD_ADDR);
    Wire.write(data | LCD_ENABLE);
    Wire.endTransmission();
    delayMicroseconds(1);
    Wire.beginTransmission(LCD_ADDR);
    Wire.write(data & ~LCD_ENABLE);
    Wire.endTransmission();
    delayMicroseconds(100);
}

void lcdSend(uint8_t value, uint8_t mode) {
    lcdSendNibble(value & 0xF0, mode);
    lcdSendNibble((value << 4) & 0xF0, mode);
}

inline void lcdCmd(uint8_t value) {
    lcdSend(value, false);
}

inline void lcdData(uint8_t value) {
    lcdSend(value, true);
}
} // namespace

void Lcd::init() {
    delay(50);
    // set 4-bit mode
    lcdSendNibble(0x30, 0);
    delay(5);
    lcdSendNibble(0x30, 0);
    delayMicroseconds(150);
    lcdSendNibble(0x20, 0);

    lcdCmd(0x28); // two strings, 4-bit
    lcdCmd(0x0C); // display is on, cursor is off
    lcdCmd(0x06); // cursor to the left after printing
    lcdCmd(0x01); // clear display
    delay(2);     // delay for long clear display command
}

void Lcd::setCursor(uint8_t c, uint8_t r) {
    static uint8_t rowOffsets[] = { 0x00, 0x40 };
    lcdCmd(0x80 | (c + rowOffsets[r]));
}

void Lcd::print(const char* str) {
    while (*str)
        lcdData(*str++);
}

#endif
