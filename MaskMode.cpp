#include "MaskMode.h"

#include "Tools.h"

namespace {
constexpr uint8_t gMasksMaxNumber = 10;

// Ardruino cpp library haven't std::function,
// so it's pretty hard to pass lambda that capture something
// We need this vars inside so let's make them static objects
uint32_t gMasks[gMasksMaxNumber];
} // namespace

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(gMasks, 0, sizeof(gMasks));
    gMasks[0] = 8 * 1000;
    gMasks[1] = 2 * 1000;
    m_step = Step::setNum;
}

void MaskMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        if (m_step == Step::setMasks) {
            if (m_currentMask + 1 == m_numberOfMasks) {
                m_step = Step::log;
            } else {
                ++m_currentMask;
                if (m_currentMask > 1 && gMasks[m_currentMask] == 0)
                    gMasks[m_currentMask] = gMasks[m_currentMask - 1];
            }
        } else {
            m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
            m_currentMask = 0;
        }
        gTimer.resetTotal();
    }

    switch (m_step) {
    case Step::setNum: {
        printFormatedLine("Mask printing", 0);
        getInt(m_numberOfMasks, 2, gMasksMaxNumber);
        char str[MAX_SYMS_PER_LINE + 1] = "Masks num: ";
        concatInt(str, m_numberOfMasks);
        printFormatedLine(str, 1);
    }
        return;
    case Step::setMasks: {
        char str[MAX_SYMS_PER_LINE + 1] = "Mask set: #";
        concatInt(str, m_currentMask + 1);
        printFormatedLine(str, 0);
        getTime(gMasks[m_currentMask]);
        printFormatedTime("", gMasks[m_currentMask]);
    }
        return;
    case Step::log:
        printTimeLog(
            "M Log ", [](uint8_t N) -> uint32_t { return gMasks[N]; }, m_numberOfMasks);
        return;
    case Step::run:
        break;
    }

    char str[MAX_SYMS_PER_LINE + 1] = "Mask #";
    concatInt(str, m_currentMask + 1 - (gTimer.state() == Timer::RUNNING));
    concat(str, " T:");
    concatTime(str, gTimer.total());
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::RUNNING) {
        printFormatedTime("", gTimer.left());
    } else if (m_currentMask == m_numberOfMasks) {
        printFormatedLine("Finish", 1);
        return;
    } else {
        printFormatedTime("", gMasks[m_currentMask]);
    }

    if (gStartBtn.click()) {
        if (gTimer.state() == Timer::STOPPED) {
            gTimer.start(gMasks[m_currentMask]);
            ++m_currentMask;
        }
    }
}

void MaskMode::reset() {
    if (m_step != Step::run)
        return;

    gTimer.stop();
    gTimer.resetTotal();
    m_currentMask = 0;
}
