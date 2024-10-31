#include "TimeTable.h"

#include <assert.h>

#include "Tools.h"

void TimeTable::empty() {
    m_size = 0;
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

void TimeTable::forcePaint() {
    m_changed = false;
    gDisplay.reset();

    m_currentShift = -1;
    m_currentLine = -1;
    uint8_t id = 0;

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        uint8_t lineLen = 0;
        if (row == 0) {
            gDisplay[0] << m_prefix;
            lineLen = strlen(m_prefix);
        }

        while (true) {
            if (id == m_size)
                return;

            char str[DISPLAY_COLS + 1] = { 0 };
            bool current = id == m_currentId;
            uint8_t alignSize = 0;
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
                m_currentAlign = alignSize = timeLen;
            }

            if (lineLen + timeLen > DISPLAY_COLS)
                break;

            gDisplay[row].print(str, current, alignSize, mark);
            if (current) {
                m_currentLine = row;
                m_currentShift = lineLen;
            }

            lineLen += timeLen + 1;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
void TimeTable::paint() {
    if (!m_changed)
        paintUnchanged();
    else
        forcePaint();
}

void TimeTable::paintUnchanged() const {
    if (!currentIsPrinted())
        return;

    if (gTimer.state() != Timer::RUNNING) {
        if (gTimer.stopped())
            gDisplay[m_currentLine].restore();
        return;
    }

    char str[DISPLAY_COLS + 1];

    if (gTimer.lag()) {
        strcpy(str, "Lag");
    } else {
        auto time = Time::fromMillis(gTimer.left());
        time.getFormatedTime(str, true, true);
    }

    char alignedStr[DISPLAY_COLS + 1];
    memset(alignedStr, ' ', m_currentAlign);
    uint8_t len = strlen(str);
    strcpy(alignedStr + m_currentAlign - len, str);

    gDisplay[m_currentLine].resetBlink();
    gDisplay[m_currentLine].fastRepaint(alignedStr, m_currentShift);
}
