#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(m_masks, 0, sizeof(m_masks));
    m_masks[0] = 80_ts;
    m_step = Step::setNum;
}

void MaskMode::process() {
    if (gModeSwitchBtn.click() && gTimer.state() != Timer::RUNNING) {
        if (m_step == Step::setMasks) {
            if (m_currentMask + 1 == m_numberOfMasks) {
                m_step = Step::run;
                m_currentMask = 0;
            } else {
                ++m_currentMask;
                if (m_currentMask > 0 && !m_masks[m_currentMask]) {
                    if (m_currentMask == 1)
                        m_masks[m_currentMask] = m_masks[0] / 4;
                    else
                        m_masks[m_currentMask] = m_masks[m_currentMask - 1];
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
        getTime(m_masks[m_currentMask]);
        gDisplay[1] << m_masks[m_currentMask];
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
        gDisplay[1] << m_masks[m_currentMask];
    }

    if (gStartBtn.click()) {
        if (gTimer.state() == Timer::STOPPED) {
            gTimer.start(m_masks[m_currentMask]);
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

void MaskMode::printLog() const {
    gDisplay[0] << "M Log ";
    uint8_t id = 0;

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        while (true) {
            if (id == m_numberOfMasks)
                break;

            char str[DISPLAY_COLS + 1] = { 0 };
            Time time = m_masks[id];
            time.getFormatedTime(str);
            if (!gDisplay[row].tryPrint(str))
                break;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
