#pragma once

#include <stdint.h>

class Lcd {
public:
    // should be called at setup function
    void init();

    void setCursor(uint8_t c, uint8_t r);
    void print(const char* str);
    void addCustomChar(uint8_t location, const uint8_t (&matrix)[8]);
};
