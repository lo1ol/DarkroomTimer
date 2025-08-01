#include "ScrollableContent.h"

#include "Tools.h"

void ScrollableContent::reset() {
    m_currentLine = -1;
    m_line = 0;
    m_lastLineLength = 0;
    m_lines[0][0] = 0;
    m_changed = true;
}

void ScrollableContent::scroll(int8_t dir) {
    if (!dir)
        return;

    if (lineCnt() <= DISPLAY_ROWS)
        return;

    if (m_firstPrintedLine == 0 && dir == -1)
        return;

    if (m_firstPrintedLine + DISPLAY_ROWS >= lineCnt() && dir == 1)
        return;

    m_firstPrintedLine += dir;
    m_changed = true;
}

void ScrollableContent::print(const char* src, bool current, const char* mark) {
    m_changed = true;

    uint8_t srclen = strlen(src);
    if ((srclen + m_lastLineLength + 1) > DISPLAY_COLS)
        startNewLine();

    if (m_lastLineLength)
        m_lines[m_line][m_lastLineLength++] = ' ';

    memcpy(m_lines[m_line] + m_lastLineLength, src, srclen + 1);
    m_lastLineLength += srclen;

    if (!current)
        return;

    m_currentLine = m_line;
    m_currentAlign = srclen;
    m_currentShift = m_lastLineLength - m_currentAlign;
    m_currentMark = mark;
    m_needGoToCurrent = true;
}

void ScrollableContent::startNewLine() {
    if (m_lastLineLength)
        ++m_line;

    m_lines[m_line][0] = 0;
    m_lastLineLength = 0;
    m_changed = true;
}

void ScrollableContent::forcePaint() {
    m_changed = false;
    gDisplay.reset();

    if (m_needGoToCurrent && m_currentLine != -1) {
        m_needGoToCurrent = false;
        if (m_currentLine < m_firstPrintedLine)
            m_firstPrintedLine = m_currentLine;
        else if (m_firstPrintedLine + DISPLAY_ROWS <= m_currentLine)
            m_firstPrintedLine = m_currentLine + 1 - DISPLAY_ROWS;
    }

    m_currentDisplayLine = -1;

    if (m_firstPrintedLine >= m_line)
        m_firstPrintedLine = 0;

    for (uint8_t displayLine = 0; displayLine != DISPLAY_ROWS; ++displayLine) {
        uint8_t lineId = m_firstPrintedLine + displayLine;
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
