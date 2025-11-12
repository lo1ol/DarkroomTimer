#pragma once

#define A0 14
#define A1 15
#define A2 16
#define A3 17
#define A4 18

#define OUTPUT 0
#define F(x) (x)

#define pinMode(pin, mode)
#define noInterrupts()
#define interrupts()

using __FlashStringHelper = char;

void itoa(int n, char s[], int base);

inline int min(int x, int y) {
    return (x > y) ? y : x;
}
