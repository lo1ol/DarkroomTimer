#include "../Config.h"

#if LCD_VERSION == LCD_VERSION_DIRECT && !defined(PIO_UNIT_TESTING)

    #include "Direct.h"

    #include <GyverIO.h>

namespace {

void lcdPulseEnable() {
    gio::write(LCD_EN_PIN, true);
    delayMicroseconds(1);
    gio::write(LCD_EN_PIN, false);
    delayMicroseconds(100);
}

void lcdWrite4bits(uint8_t value) {
    gio::write(LCD_D4_PIN, (value >> 0) & 0x01);
    gio::write(LCD_D5_PIN, (value >> 1) & 0x01);
    gio::write(LCD_D6_PIN, (value >> 2) & 0x01);
    gio::write(LCD_D7_PIN, (value >> 3) & 0x01);
    lcdPulseEnable();
}

void lcdSend(uint8_t value, bool rs) {
    gio::write(LCD_RS_PIN, rs);
    lcdWrite4bits(value >> 4);
    lcdWrite4bits(value);
}

inline void lcdCmd(uint8_t cmd) {
    lcdSend(cmd, false);
}

inline void lcdData(uint8_t data) {
    lcdSend(data, true);
}

} // namespace

void Lcd::init() {
    pinMode(LCD_RS_PIN, OUTPUT);
    pinMode(LCD_EN_PIN, OUTPUT);
    pinMode(LCD_D4_PIN, OUTPUT);
    pinMode(LCD_D5_PIN, OUTPUT);
    pinMode(LCD_D6_PIN, OUTPUT);
    pinMode(LCD_D7_PIN, OUTPUT);

    delay(50);

    lcdWrite4bits(0x03);
    delay(5);
    lcdWrite4bits(0x03);
    delayMicroseconds(150);
    lcdWrite4bits(0x03);
    lcdWrite4bits(0x02);

    lcdCmd(0x28);
    lcdCmd(0x0C);
    lcdCmd(0x06);
    lcdCmd(0x01);
    delay(2);
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
