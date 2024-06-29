#include "MaskMode.h"

#include "../DisplayLine.h"
#include "../Tools.h"

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
    m_step = Step::setNum;
}

void MaskMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        if (m_step == Step::setMasks) {
            if (m_currentMask + 1 == m_numberOfMasks) {
                m_step = Step::log;
            } else {
                ++m_currentMask;
                if (m_currentMask > 0 && gMasks[m_currentMask] == 0) {
                    if (m_currentMask == 1)
                        gMasks[m_currentMask] = (gMasks[0] / 400) * 100;
                    else
                        gMasks[m_currentMask] = gMasks[m_currentMask - 1];
                }
            }
        } else {
            m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
            m_currentMask = 0;
        }
        gTimer.resetTotal();
    }

    switch (m_step) {
    case Step::setNum:
        gDisplay[0] << "Mask printing";
        getInt(m_numberOfMasks, 2, gMasksMaxNumber);
        gDisplay[1] << "Masks num: " << m_numberOfMasks;
        return;
    case Step::setMasks:
        gDisplay[0] << "Mask set: #" << (m_currentMask + 1);
        getTime(gMasks[m_currentMask]);
        gDisplay[1] << gMasks[m_currentMask];
        return;
    case Step::log:
        gDisplay.printTimeLog(
            "M Log ", [](uint8_t N) -> uint32_t { return gMasks[N]; }, m_numberOfMasks);
        return;
    case Step::run:
        break;
    }

    int run = (m_currentMask + 1 - (gTimer.state() == Timer::RUNNING));
    gDisplay[0] << "Mask #" << run << " T:" << gTimer.total();

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
    } else if (m_currentMask == m_numberOfMasks) {
        gDisplay[1] << "Finish";
        return;
    } else {
        gDisplay[1] << gMasks[m_currentMask];
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
