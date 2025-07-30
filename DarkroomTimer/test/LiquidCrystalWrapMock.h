#pragma once

#include <assert.h>
#include <string.h>

#include <Config.h>
#include <LiquidCrystalWrap.h>

class LiquidCrystalWrapMock : public LiquidCrystalWrap {
public:
    void print(const char* str) override {
        auto len = strlen(str);
        assert((m_c + len) <= DISPLAY_COLS);
        memcpy(m_lines[m_r] + m_c, str, len);
    }

    void setCursor(uint8_t c, uint8_t r) override {
        m_c = c;
        m_r = r;
    }

    const char* getLine(uint8_t r) const { return m_lines[r]; }

private:
    uint8_t m_c = 0;
    uint8_t m_r = 0;

    char m_lines[DISPLAY_ROWS][DISPLAY_COLS + 1] = {};
};
