#pragma once

#include <stdint.h>
#include <string.h>

class Lcd {
public:
    void init() {}

    void clear() { memset(m_lines, 0, sizeof(m_lines)); }

    void beginFastPrint() { m_inFastPrint = true; }

    void endFastPrint() { m_inFastPrint = false; }

    void print(const char* str) {
        auto len = strlen(str);
        assert((m_c + len) <= DISPLAY_COLS);
        memcpy(m_lines[m_r] + m_c, str, len);
        ++m_printCallCount;

        m_c += len;
    }

    void print(char ch) {
        assert(m_c < DISPLAY_COLS);
        m_lines[m_r][m_c] = ch;
        ++m_c;
    }

    void setCursor(uint8_t c, uint8_t r) {
        assert(c < DISPLAY_COLS);
        assert(r < DISPLAY_ROWS);
        m_c = c;
        m_r = r;
    }

    void addPROGMEMCustomChar(uint8_t location, const uint8_t (&matrix)[8]) {
        assert(location >= 0x80 && location < 0x88);
    }
    void addRAMCustomChar(uint8_t location, const uint8_t (&matrix)[8]) { assert(location >= 0x80 && location < 0x88); }

    [[nodiscard]] const char* getLine(uint8_t r) const { return m_lines[r]; }

    [[nodiscard]] uint8_t printCallCount() {
        auto res = m_printCallCount;
        m_printCallCount = 0;
        return res;
    }

    void resetCallCount() { m_printCallCount = 0; }
    bool inFastPrint() const { return m_inFastPrint; }
    bool empty() const {
        for (auto line : m_lines)
            for (uint8_t i = 0; i != DISPLAY_COLS; ++i)
                if (line[i] != 0 && line[i] != ' ')
                    return false;
        return true;
    }

private:
    uint8_t m_c = 0;
    uint8_t m_r = 0;
    uint8_t m_printCallCount = 0;
    bool m_inFastPrint = false;

    char m_lines[DISPLAY_ROWS][DISPLAY_COLS + 1] = {};
};

