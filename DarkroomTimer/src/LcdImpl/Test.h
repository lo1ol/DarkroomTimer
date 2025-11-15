#pragma once

#include <stdint.h>
#include <string.h>

class Lcd {
public:
    void print(const char* str) {
        auto len = strlen(str);
        assert((m_c + len) <= DISPLAY_COLS);
        memcpy(m_lines[m_r] + m_c, str, len);
        ++m_printCallCount;
    }

    void setCursor(uint8_t c, uint8_t r) {
        m_c = c;
        m_r = r;
    }

    void addCustomChar(uint8_t location, const uint8_t (&matrix)[8]) {}
    [[nodiscard]] const char* getLine(uint8_t r) const { return m_lines[r]; }

    [[nodiscard]] uint8_t printCallCount() {
        auto res = m_printCallCount;
        m_printCallCount = 0;
        return res;
    }

    void resetCallCount() { m_printCallCount = 0; }

private:
    uint8_t m_c = 0;
    uint8_t m_r = 0;
    uint8_t m_printCallCount = 0;

    char m_lines[DISPLAY_ROWS][DISPLAY_COLS + 1] = {};
};

