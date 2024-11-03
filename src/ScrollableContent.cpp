#include "ScrollableContent.h"

#include "Tools.h"

void ScrollableContent::reset() {
    m_currentLine = -1;
    m_line = 0;
    m_lastLineIsEmpty = true;
    m_lines[0][0] = 0;
    m_changed = true;
}

void ScrollableContent::scroll() {
    if (gTimer.state() == Timer::RUNNING)
        return;

    auto dir = getEncoderDir();
    if (!dir)
        return;

    if (lineCnt() <= DISPLAY_ROWS)
        return;

    if (m_fistPrintedLine == 0 && dir == -1)
        return;

    if (m_fistPrintedLine + DISPLAY_ROWS >= lineCnt() && dir == 1)
        return;

    m_fistPrintedLine += dir;
    m_changed = true;
}

void ScrollableContent::print(const char* src, bool current, const char* mark) {
    m_changed = true;
    m_lastLineIsEmpty = false;

    strcat(m_lines[m_line], src);
    if (!current)
        return;

    m_currentLine = m_line;
    m_currentAlign = strlen(src);
    m_currentShift = strlen(m_lines[m_line]) - m_currentAlign;
    m_currentMark = mark;
    m_needGoToCurrent = true;
}

void ScrollableContent::startNewLine() {
    if (!m_lastLineIsEmpty)
        ++m_line;

    m_lines[m_line][0] = 0;
    m_lastLineIsEmpty = true;
    m_changed = true;
}

void ScrollableContent::forcePaint() {
    m_changed = false;
    gDisplay.reset();

    if (m_needGoToCurrent && m_currentLine != -1) {
        m_needGoToCurrent = false;
        if (m_currentLine < m_fistPrintedLine)
            m_fistPrintedLine = m_currentLine;
        else if (m_fistPrintedLine + DISPLAY_ROWS <= m_currentLine)
            m_fistPrintedLine = m_currentLine + 1 - DISPLAY_ROWS;
    }

    m_currentDisplayLine = -1;

    if (m_fistPrintedLine >= m_line)
        m_fistPrintedLine = 0;

    for (uint8_t displayLine = 0; displayLine != DISPLAY_ROWS; ++displayLine) {
        uint8_t lineId = m_fistPrintedLine + displayLine;
        if (lineId == lineCnt())
            return;

        if (lineId != m_currentLine) {
            gDisplay[displayLine] << m_lines[lineId];
            continue;
        }

        m_currentDisplayLine = displayLine;
        char symBackup = m_lines[lineId][m_currentShift];
        m_lines[lineId][m_currentShift] = 0;

        gDisplay[displayLine] << m_lines[lineId];
        m_lines[lineId][m_currentShift] = symBackup;
        symBackup = m_lines[lineId][m_currentShift + m_currentAlign];
        m_lines[lineId][m_currentShift + m_currentAlign] = 0;

        gDisplay[displayLine].print(m_lines[lineId] + m_currentShift, true, m_currentMark);

        m_lines[lineId][m_currentShift + m_currentAlign] = symBackup;
        gDisplay[displayLine] << (m_lines[lineId] + m_currentShift + m_currentAlign);
    }
}

void ScrollableContent::paintUnchanged() {
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

    alignStr(str, m_currentAlign);

    gDisplay[m_currentDisplayLine].resetBlink();
    gDisplay[m_currentDisplayLine].fastRepaint(str, m_currentShift);
}

void ScrollableContent::paint() {
    if (m_changed)
        forcePaint();
    else
        paintUnchanged();
}
