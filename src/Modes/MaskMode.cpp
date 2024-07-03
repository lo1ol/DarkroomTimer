#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(m_masks, 0, sizeof(m_masks));
    m_masks[0] = 80_ts;
    m_step = Step::setNum;
    m_currentMask = 0;
}

void MaskMode::switchMode() {
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

    gTimer.reset();
}

void MaskMode::process() {
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

    gDisplay[0] << "Mask #" << m_currentMask + 1 << " T:" << gTimer.total();

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
    } else if (m_currentMask == m_numberOfMasks) {
        gDisplay[1] << "Finish";
        return;
    } else {
        gDisplay[1] << m_masks[m_currentMask];
    }

    processRun();
}

void MaskMode::processRun() {
    if (gTimer.stopped())
        ++m_currentMask;

    if (!gStartBtn.click())
        return;

    if (gTimer.state() != Timer::STOPPED)
        return;

    gTimer.start(m_masks[m_currentMask]);
}

void MaskMode::reset() {
    if (m_step != Step::run)
        return;

    m_currentMask = 0;
}

void MaskMode::printLog() {
    gDisplay[0] << "M Log ";

    uint8_t id = printLogHelper(
        [](void* this__, uint8_t id, bool& current, bool& end) -> Time {
            auto this_ = reinterpret_cast<MaskMode*>(this__);

            current = this_->m_step == Step::run && this_->m_currentMask == id;
            end = id == this_->m_numberOfMasks;
            if (end)
                return {};

            return { this_->m_masks[id] };
        },
        this);

    if (m_currentMask < id && m_step == Step::run)
        processRun();
}
