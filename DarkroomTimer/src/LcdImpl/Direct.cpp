#include "../Config.h"

#if LCD_VERSION == LCD_VERSION_DIRECT && !defined(PIO_UNIT_TESTING)

    #include "Direct.h"

    #include "../CustomLcdSyms.h"

    #include <GyverIO.h>

    #define LCD_SETCGRAMADDR 0x40
    #define LCD_SETDDRAMADDR 0x80

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

    // first try to set 4-bit mode
    lcdWrite4bits(0x03);
    delay(5);
    // second
    lcdWrite4bits(0x03);
    delay(5);
    // third
    lcdWrite4bits(0x03);
    delayMicroseconds(150);

    lcdWrite4bits(0x03);
    lcdWrite4bits(0x02);

    lcdCmd(0x28); // two strings, 4-bit
    lcdCmd(0x0C); // display is on, cursor is off
    lcdCmd(0x01);
    delay(2);

    lcdCmd(0x06);

    // syms id are shifted by one
    createChar(kWplusSym - 1, kWplusMatrix);
    createChar(kWminusSym - 1, kWminusMatrix);
    createChar(kYplusSym - 1, kYplusMatrix);
    createChar(kYminusSym - 1, kYminusMatrix);
    createChar(kMplusSym - 1, kMplusMatrix);
    createChar(kMminusSym - 1, kMminusMatrix);
    createChar(kCplusSym - 1, kCplusMatrix);
    createChar(kCminusSym - 1, kCminusMatrix);
}

void Lcd::setCursor(uint8_t c, uint8_t r) {
    static uint8_t rowOffsets[] = { 0x00, 0x40 };
    lcdCmd(0x80 | (c + rowOffsets[r]));
}

void Lcd::print(const char* str) {
    while (char ch = *str++) {
        // custom char
        if (ch < 9)
            --ch;
        lcdData(ch);
    }
}

void Lcd::createChar(uint8_t symId, const uint8_t symMatrix[8]) {
    lcdCmd(LCD_SETCGRAMADDR | (symId << 3));

    for (uint8_t i = 0; i != 8; i++)
        lcdData(symMatrix[i]);
}

#endif
