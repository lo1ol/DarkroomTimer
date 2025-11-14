#include "../Config.h"

#if LCD_VERSION == LCD_VERSION_I2C && !defined(PIO_UNIT_TESTING)

    #include "I2C.h"

    #define LCD_ADDR 0x27
    #define LCD_BACKLIGHT 0x08
    #define LCD_ENABLE 0x04
    #define LCD_RS 0x01
    #define LCD_CLEARDISPLAY 0x01

namespace {

// Simple I2C implementation
// For more info:
// https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
// Page 183
void i2cInit() {
    TWSR = 0;
    TWBR = 72; // ~100kHz @16MHz
}

void i2cStart(uint8_t addr) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    TWDR = addr;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
}

void i2cWrite(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
}

void i2cStop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void lcdSendNibble(uint8_t data, bool isData) {
    data |= LCD_BACKLIGHT;

    if (isData)
        data |= LCD_RS;

    // write data with pulse enable
    // https://github.com/arduino-libraries/LiquidCrystal/blob/8c18c69e9de2fb7d89e64098b1899064fee3ba06/src/LiquidCrystal.cpp#L303
    i2cWrite(data | LCD_ENABLE);
    delayMicroseconds(1);
    i2cWrite(data & ~LCD_ENABLE);
    delayMicroseconds(50);
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
    // order is taken from
    // https://github.com/johnrickman/LiquidCrystal_I2C/blob/738765e388816fb2a687d40436cd2604febce1b1/LiquidCrystal_I2C.cpp#L71
    i2cInit();
    delay(50); // some time to wake up device

    i2cStart(LCD_ADDR << 1);
    // set 4-bit mode
    lcdSendNibble(0x30, 0);
    delay(5);

    // second try
    lcdSendNibble(0x30, 0);
    delay(5);

    // second try
    lcdSendNibble(0x30, 0);
    delayMicroseconds(150);

    delayMicroseconds(150);
    lcdSendNibble(0x20, 0);

    lcdCmd(0x28); // two strings, 4-bit
    lcdCmd(0x0C); // display is on, cursor is off

    lcdCmd(LCD_CLEARDISPLAY); // clear display
    delay(2);                 // delay for long clear display command

    lcdCmd(0x06); // cursor to the left after printing
    i2cStop();
}

void Lcd::setCursor(uint8_t c, uint8_t r) {
    i2cStart(LCD_ADDR << 1);
    static uint8_t rowOffsets[] = { 0x00, 0x40 };
    lcdCmd(0x80 | (c + rowOffsets[r]));
    i2cStop();
}

void Lcd::print(const char* str) {
    i2cStart(LCD_ADDR << 1);
    while (uint8_t ch = *str++) {
        if (ch >= 0x80)
            ch -= 0x80;
        lcdData(ch);
    }
    i2cStop();
}

void Lcd::addCustomChar(uint8_t location, const uint8_t (&matrix)[8]) {
    i2cStart(LCD_ADDR << 1);
    lcdCmd(0x40 | location << 3);
    for (uint8_t i = 0; i != sizeof(matrix); ++i)
        lcdData(pgm_read_byte(matrix + i));
    i2cStop();
}

#endif
