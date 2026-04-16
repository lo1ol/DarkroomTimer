#pragma once

#include "../CustomChars.h"

class Lcd {
public:
    void init();
    void setCursor(uint8_t c, uint8_t r);
    void print(const char* str);
    void print(char ch);
    void clear();
    // add custom char from program memory
    void addPROGMEMCustomChar(uint8_t location, const uint8_t (&matrix)[8]);
    // add custom char from RAM
    void addRAMCustomChar(uint8_t location, const uint8_t (&matrix)[8]);

    void beginFastPrint() {}
    void endFastPrint() {}
};
