#pragma once

class Lcd {
public:
    void init();
    void setCursor(uint8_t c, uint8_t r);
    void print(const char* str);
};
