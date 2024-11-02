#include "TimeTable.h"

#include <assert.h>

#include "Tools.h"

void TimeTable::empty() {
    m_size = 0;
    m_fistPrintedLine = 0;
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
    m_currentShift = -1;
    m_currentLine = -1;
    m_changed = true;

    if (m_currentId != -1)
        m_needGoToCurrent = true;
}

void TimeTable::scroll() {
    if (gTimer.state() == Timer::RUNNING)
        return;

    auto dir = getEncoderDir();
    if (!dir)
        return;

    if (m_maxLineNum <= DISPLAY_ROWS)
        return;

    if (m_fistPrintedLine == 0 && dir == -1)
        return;

    if (m_fistPrintedLine + DISPLAY_ROWS >= m_maxLineNum && dir == 1)
        return;

    m_fistPrintedLine += dir;
    m_changed = true;
}

void TimeTable::forcePaint() {
tryAgain:
    m_changed = false;
    gDisplay.reset();

    m_currentDisplayLine = -1;
    uint8_t id = 0;
    uint8_t lineId = 0;

    while (true) {
        int8_t printedDisplayLine = -1;
        if (lineId >= m_fistPrintedLine && lineId - m_fistPrintedLine < DISPLAY_ROWS)
            printedDisplayLine = lineId - m_fistPrintedLine;

        bool exit = false;

        uint8_t lineLen = 0;
        if (lineId == 0) {
            if (printedDisplayLine != -1)
                gDisplay[printedDisplayLine] << m_prefix;

            lineLen = strlen(m_prefix);
        }

        while (true) {
            if (id == m_size) {
                exit = true;
                break;
            }

            char str[DISPLAY_COLS + 1] = { 0 };
            bool current = id == m_currentId;
            uint8_t alignSize = 0;
            const char* mark = nullptr;

            auto time = m_times[id];

            if (time == kBadTime) {
                if (m_printBadAsZero)
                    time = 0_s;
                else {
                    exit = true;
                    break;
                }
            }

            time.getFormatedTime(str, current, current);

            auto timeLen = strlen(str);
            if (current) {
                mark = m_currentMark;
                m_currentAlign = alignSize = timeLen;
            }

            if (lineLen + timeLen > DISPLAY_COLS)
                break;

            if (printedDisplayLine != -1)
                gDisplay[printedDisplayLine].print(str, current, alignSize, mark);

            if (current) {
                m_currentDisplayLine = printedDisplayLine;
                m_currentLine = lineId;
                m_currentShift = lineLen;
                if (m_needGoToCurrent && m_currentDisplayLine == -1) {
                    m_needGoToCurrent = false;
                    if (lineId < m_fistPrintedLine)
                        m_fistPrintedLine = lineId;
                    else
                        m_fistPrintedLine = lineId + 1 - DISPLAY_ROWS;

                    goto tryAgain;
                }
            }

            lineLen += timeLen + 1;

            if (printedDisplayLine != -1)
                gDisplay[printedDisplayLine] << " ";
            ++id;
        }

        if (!exit) {
            ++lineId;
            continue;
        }

        m_maxLineNum = lineId + 1;
        if (!lineLen)
            --m_maxLineNum;

        return;
    }
}
void TimeTable::paint() {
    if (m_changed)
        forcePaint();
    else
        paintUnchanged();
}

void TimeTable::paintUnchanged() {
    if (!currentIsPrinted())
        return;

    if (m_currentDisplayLine == -1) {
        if (gTimer.state() == Timer::RUNNING) {
            m_needGoToCurrent = true;
            m_changed = true;
        }
        return;
    }

    if (gTimer.state() != Timer::RUNNING) {
        if (gTimer.stopped())
            gDisplay[m_currentDisplayLine].restore();
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

    gDisplay[m_currentDisplayLine].resetBlink();
    gDisplay[m_currentDisplayLine].fastRepaint(alignedStr, m_currentShift);
}
