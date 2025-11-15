#pragma once

#include <Arduino.h>
#include <stdint.h>

enum class Charset : uint8_t {
    Main,
    ModePreview,
    View,
};

// Thank you for https://chareditor.com

// syms from main charset
// clang-format off
constexpr uint8_t kWrenchSymTopMatrix[] PROGMEM = {
    0b01100, 0b11000, 0b11001, 0b11111, 0b01110, 0b01100, 0b01100, 0b01100
};
constexpr uint8_t kWrenchSymBottomMatrix[] PROGMEM = {
    0b01100, 0b01100, 0b01100, 0b01110, 0b11111, 0b10011, 0b00011, 0b00110
};
constexpr uint8_t kSheetSymMatrix[] PROGMEM = {
    0b11100, 0b10110, 0b10111, 0b10001, 0b10001, 0b10001, 0b11111, 0b00000
};
constexpr uint8_t kPlaySymMatrix[] PROGMEM = {
    0b10000, 0b11000, 0b11100, 0b11110, 0b11100, 0b11000, 0b10000, 0b00000
};
constexpr uint8_t kPauseSymMatrix[] PROGMEM = {
    0b00000, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b11011, 0b00000
};
// clang-format on

#define kWrenchSymTop "\x80"
#define kWrenchSymBottom "\x81"
#define kSheetSym "\x82"
#define kPlaySym "\x83"
#define kPauseSym "\x84"

// syms from mode preview charset
// clang-format off
constexpr uint8_t kFolderBegin1SymMatrix[] PROGMEM = {
    0b11100, 0b10010, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111
};
constexpr uint8_t kFolderEnd1SymMatrix[] PROGMEM = {
    0b00000, 0b00000, 0b11100, 0b00010, 0b00010, 0b00010, 0b00010, 0b11110
};
constexpr uint8_t kFolderBegin2SymMatrix[] PROGMEM = {
    0b01110, 0b01001, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111
};
#define kFolderEnd2SymMatrix kFolderEnd1SymMatrix
constexpr uint8_t kFolderBegin3SymMatrix[] PROGMEM = {
    0b00111, 0b00100, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111
};
constexpr uint8_t kFolderEnd3SymMatrix[] PROGMEM = {
    0b00000, 0b10000, 0b11100, 0b00010, 0b00010, 0b00010, 0b00010, 0b11110
};
constexpr uint8_t kFolderBegin4SymMatrix[] PROGMEM = {
    0b00011, 0b00010, 0b11111, 0b10000, 0b10000, 0b10000, 0b10000, 0b11111
};
constexpr uint8_t kFolderEnd4SymMatrix[] PROGMEM = {
    0b10000, 0b01000, 0b11100, 0b00010, 0b00010, 0b00010, 0b00010, 0b11110
};

constexpr uint8_t kSubFolderPointerSymMatrix[] PROGMEM = {
    0b00000, 0b01000, 0b01000, 0b01000, 0b01110, 0b00000, 0b00000, 0b00000
};
// clang-format on

#define kFolderBegin1Sym "\x80"
#define kFolderEnd1Sym "\x81"
#define kFolderBegin2Sym "\x82"
#define kFolderEnd2Sym "\x81"
#define kFolderBegin3Sym "\x83"
#define kFolderEnd3Sym "\x84"
#define kFolderBegin4Sym "\x85"
#define kFolderEnd4Sym "\x86"
#define kSubFolderPointerSym "\x87"

#define kFolder1Str kFolderBegin1Sym kFolderEnd1Sym
#define kFolder2Str kFolderBegin2Sym kFolderEnd2Sym
#define kFolder3Str kFolderBegin3Sym kFolderEnd3Sym
#define kFolder4Str kFolderBegin4Sym kFolderEnd4Sym

// syms from view charset
// clang-format off
constexpr uint8_t kLampSymMatrix[] PROGMEM = {
    0b01110, 0b01110, 0b11111, 0b01010, 0b10001, 0b10001, 0b10001, 0b01110
};
constexpr uint8_t kLampLight1SymMatrix[] PROGMEM = {
    0b00000, 0b01010, 0b10101, 0b00100, 0b00000, 0b00000, 0b00000, 0b11111
};
constexpr uint8_t kLampLight2SymMatrix[] PROGMEM = {
    0b00000, 0b00000, 0b01010, 0b10001, 0b00100, 0b00100, 0b00000, 0b11111
};
constexpr uint8_t kLampLight3SymMatrix[] PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b01010, 0b10001, 0b10101, 0b00100, 0b11111
};
constexpr uint8_t kLampLight4SymMatrix[] PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b01010, 0b10001, 0b10101, 0b11111
};
constexpr uint8_t kLampLight5SymMatrix[] PROGMEM = {
    0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b01010, 0b10001, 0b11111
};
constexpr uint8_t kLampLight6SymMatrix[] PROGMEM = {
    0b00100, 0b01010, 0b00000, 0b00000, 0b00000, 0b00000, 0b01010, 0b11111
};
// clang-format on

#define kLampSym "\x80"
#define kLampLight1Sym "\x81"
#define kLampLight2Sym "\x82"
#define kLampLight3Sym "\x83"
#define kLampLight4Sym "\x84"
#define kLampLight5Sym "\x85"
#define kLampLight6Sym "\x86"
