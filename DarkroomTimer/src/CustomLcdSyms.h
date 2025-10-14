#pragma once

#include <stdint.h>

// only values between 1 and 8 is allowed
// this values will be decremented by LcdImpl during writing
constexpr char kWplusSym = 1;
constexpr char kWminusSym = 2;
constexpr char kYplusSym = 3;
constexpr char kYminusSym = 4;
constexpr char kMplusSym = 5;
constexpr char kMminusSym = 6;
constexpr char kCplusSym = 7;
constexpr char kCminusSym = 8;

// clang-format off
constexpr uint8_t kWplusMatrix[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b00000,
    0b10001,
    0b10001,
    0b10101,
    0b01010,
};
constexpr uint8_t kWminusMatrix[8] = {
    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b10001,
    0b10001,
    0b10101,
    0b01010,
};

constexpr uint8_t kYplusMatrix[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b00000,
    0b10001,
    0b01010,
    0b00100,
    0b00100,
};
constexpr uint8_t kYminusMatrix[8] = {
    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b10001,
    0b01010,
    0b00100,
    0b00100,
};

constexpr uint8_t kMplusMatrix[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b00000,
    0b10001,
    0b11011,
    0b10101,
    0b10001,
};
constexpr uint8_t kMminusMatrix[8] = {
    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b10001,
    0b11011,
    0b10101,
    0b10001,
};

constexpr uint8_t kCplusMatrix[8] = {
    0b00100,
    0b01110,
    0b00100,
    0b00000,
    0b01111,
    0b10000,
    0b10000,
    0b01111,
};
constexpr uint8_t kCminusMatrix[8] = {
    0b00000,
    0b01110,
    0b00000,
    0b00000,
    0b01111,
    0b10000,
    0b10000,
    0b01111,
};

// clang-format on
