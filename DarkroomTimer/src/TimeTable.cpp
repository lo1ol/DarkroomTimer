#include "TimeTable.h"

#include <assert.h>

#include "Tools.h"

void TimeTable::empty() {
    m_size = 0;
    m_currentId = -1;
    m_changed = true;
}

void TimeTable::resize(uint8_t size) {
    m_size = size;
    m_changed = true;
}

void TimeTable::setPrefix(const char* prefix) {
    m_prefix = prefix;
    m_changed = true;
}

void TimeTable::setTime(uint8_t id, Time time) {
    assert(id < kTimeTableSize);
    if (id >= m_size)
        m_size = id + 1;
    m_times[id] = time;
    m_changed = true;
}

void TimeTable::printBadAsZero(bool val) {
    m_printBadAsZero = val;
    m_changed = true;
}

void TimeTable::setCurrent(uint8_t id, const char* mark) {
    m_currentId = id;
    m_currentMark = mark;
    m_changed = true;
}

void TimeTable::flush(bool force) {
    if (!m_changed && !force)
        return;

    m_changed = false;

    uint8_t id = 0;
    bool firstLine = true;

    while (true) {
        gScrollableContent.startNewLine();

        uint8_t lineLen = 0;
        if (firstLine) {
            firstLine = false;
            gScrollableContent.print(m_prefix);

            lineLen = strlen(m_prefix);
        }

        while (true) {
            if (id == m_size) {
                return;
            }

            char str[DISPLAY_COLS + 1];
            bool current = id == m_currentId;
            const char* mark = nullptr;

            auto time = m_times[id];

            if (time == kBadTime) {
                if (m_printBadAsZero)
                    time = 0_s;
                else
                    return;
            }

            time.getFormatedTime(str, current, current);

            auto timeLen = strlen(str);
            if (current) {
                mark = m_currentMark;
            }

            if (lineLen + timeLen > DISPLAY_COLS)
                break;

            gScrollableContent.print(str, current, mark);

            lineLen += timeLen;
            ++id;

            if (lineLen + 1 > DISPLAY_COLS)
                break;

            gScrollableContent.print(" ");
            ++lineLen;
        }
    }
}

