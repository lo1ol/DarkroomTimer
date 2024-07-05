#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(m_masks, 0, sizeof(m_masks));
    m_masks[0] = 80_ts;
    m_step = Step::setNum;
    m_view = RunView::common;
    m_currentMask = 0;

    for (uint8_t i = 1; i != kMasksMaxNumber; ++i)
        m_masks[i] = -1_ts;
}

void MaskMode::switchMode() {
    gTimer.reset();

    if (m_step != Step::setMasks) {
        m_step = ADD_TO_ENUM(Step, m_step, 1);

        m_currentMask = 0;

        for (uint8_t i = m_numberOfMasks; i != kMasksMaxNumber; ++i)
            m_masks[i] = -1_ts;

        return;
    }

    if (m_currentMask + 1 == m_numberOfMasks) {
        m_step = Step::run;
        m_currentMask = 0;
        return;
    }

    ++m_currentMask;

    if (m_currentMask == 0 || m_masks[m_currentMask] != -1_ts)
        return;

    // let's guess unknown masks
    if (m_currentMask == 1)
        m_masks[m_currentMask] = m_masks[0] / 4;
    else
        m_masks[m_currentMask] = m_masks[m_currentMask - 1];
}

void MaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        gDisplay[0] << "Mask printing";
        getInt(m_numberOfMasks, 2, kMasksMaxNumber);
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

    switch (m_view) {
    case RunView::common:
        gDisplay[0] << "Mask #" << m_currentMask + 1 << " T:" << gTimer.total();

        if (gTimer.state() == Timer::RUNNING) {
            gTimer.printFormatedState();
        } else if (m_currentMask == m_numberOfMasks) {
            gDisplay[1] << "Finish";
            return;
        } else {
            gDisplay[1] << m_masks[m_currentMask];
        }

        break;
    case RunView::log: {
        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = RunView::common;
    } break;
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

void MaskMode::switchView() {
    m_view = ADD_TO_ENUM(RunView, m_view, 1);
}

bool MaskMode::canSwitchView() const {
    if (m_step != Step::run)
        return false;

    bool overFlow = false;
    printLog(overFlow);
    gDisplay.reset();
    return !overFlow;
}

void MaskMode::printLog(bool& logOverFlowed) const {
    gDisplay[0] << "M Log ";

    uint8_t id = printLogHelper(
        [](const void* this__, uint8_t id, bool& current, bool& end) -> Time {
            auto this_ = reinterpret_cast<const MaskMode*>(this__);

            current = this_->m_step == Step::run && this_->m_currentMask == id;
            end = id == this_->m_numberOfMasks;
            if (end)
                return {};

            Time time = this_->m_masks[id];
            if (time == -1_ts)
                return 0_ts;

            return time;
        },
        this);

    if (m_step == Step::run) {
        if (m_currentMask >= id && m_currentMask != m_numberOfMasks)
            logOverFlowed = true;
        else if (m_currentMask == m_numberOfMasks)
            gDisplay[1] >> " Finish";
    }
}
