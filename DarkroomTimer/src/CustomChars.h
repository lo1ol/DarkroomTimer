#pragma once

#include <stdint.h>

enum class Charset : uint8_t {
    Main,
};

// Thank you for https://chareditor.com
constexpr uint8_t kLampSymTopMatrix[] PROGMEM = {
    0b01110, 0b01110, 0b11111, 0b01010, 0b10001, 0b10001, 0b10001, 0b01110
};
constexpr uint8_t kLampSymBottomMatrix[] PROGMEM = { 0b00000, 0b01010, 0b10101, 0b00100,
                                                     0b00000, 0b00000, 0b00000, 0b00000 };
constexpr uint8_t kWrenchSymTopMatrix[] PROGMEM = { 0b01100, 0b11000, 0b11001, 0b11111,
                                                    0b01110, 0b01100, 0b01100, 0b01100 };
constexpr uint8_t kWrenchSymBottomMatrix[] PROGMEM = { 0b01100, 0b01100, 0b01100, 0b01110,
                                                       0b11111, 0b10011, 0b00011, 0b00110 };

constexpr uint8_t kSheetSymMatrix[] PROGMEM = {
    0b11100, 0b10110, 0b10111, 0b10001, 0b10001, 0b10001, 0b11111, 0b00000
};
constexpr uint8_t kPlaySymMatrix[] PROGMEM = { 0b10000, 0b11000, 0b11100, 0b11110, 0b11100, 0b11000, 0b10000, 0b00000 };

constexpr uint8_t kPauseSymMatrix[] PROGMEM = {
    0b00000, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b00000
};

// sym from main toolset
#define kLampSymTop "\x80"
#define kLampSymBottom "\x81"
#define kWrenchSymTop "\x82"
#define kWrenchSymBottom "\x83"
#define kSheetSym "\x84"
#define kPlaySym "\x85"
#define kPauseSym "\x86"
