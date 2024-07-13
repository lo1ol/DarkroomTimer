#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(m_masks, 0, sizeof(m_masks));
    m_masks[0] = 80_ts;
    m_step = Step::setNum;
    m_view = gSettings.logViewInMasks ? View::log : View::common;
    m_currentMask = 0;
    m_notifyMask = 0;

    for (uint8_t i = 1; i != kMasksMaxNumber; ++i)
        m_masks[i] = -1_ts;
}

void MaskMode::switchMode() {
    gTimer.reset();

    if (m_step != Step::setMasks) {
        m_step = ADD_TO_ENUM(Step, m_step, 1);

        m_currentMask = 0;

        m_notifyMask &= ~((~0) << m_notifyMask);
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
    m_masks[m_currentMask] = m_masks[m_currentMask - 1];
}

void MaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        gDisplay[0] << preview();
        getInt(m_numberOfMasks, 2, kMasksMaxNumber);
        gDisplay[1] << "Mask num: " << m_numberOfMasks;
        return;
    case Step::setMasks:
        processSetMasks();
        return;
    case Step::run:
        processRun();
        return;
    }
}

void MaskMode::processSetMasks() {
    if (gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        gDisplay.resetBlink(true);
    }

    bool changed = getTime(m_masks[m_currentMask]);

    switch (m_view) {
    case View::common:
        gDisplay[0] << "Mask set: #" << (m_currentMask + 1);
        gDisplay[1] << m_masks[m_currentMask];
        if (m_notifyMask & (1 << m_currentMask))
            gDisplay[1] >> "Notify";
        break;
    case View::log: {
        if (changed)
            gDisplay.resetBlink();

        gDisplay[0] << "Set ";

        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = View::common;
    } break;
    }
}

void MaskMode::processRun() {
    switch (m_view) {
    case View::common:
        gDisplay[0] << "Mask #" << m_currentMask + 1 << " T:" << gTimer.total();

        if (m_notifyMask & (1 << m_currentMask))
            gDisplay[1] >> "Notify";

        if (gTimer.state() == Timer::RUNNING) {
            gTimer.printFormatedState();
        } else if (m_currentMask == m_numberOfMasks) {
            gDisplay[1] << "Finish";
            return;
        } else {
            gDisplay[1] << m_masks[m_currentMask];
        }

        break;
    case View::log: {
        gDisplay[0] << "Run ";

        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = View::common;

        if (m_currentMask == m_numberOfMasks)
            gDisplay[1] >> " Finish";
    } break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentMask < m_numberOfMasks)
        gTimer.start(m_masks[m_currentMask]);

    if (gTimer.stopped()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm("Notification");
        ++m_currentMask;
    }
}

void MaskMode::reset() {
    m_currentMask = 0;
}

void MaskMode::switchView() {
    m_view = ADD_TO_ENUM(View, m_view, 1);
}

bool MaskMode::canSwitchView() const {
    if (m_step == Step::setNum)
        return false;

    gDisplay[0] << "Run ";
    bool overFlow = false;
    printLog(overFlow);
    gDisplay.reset();
    return !overFlow;
}

void MaskMode::printLog(bool& logOverFlowed) const {
    uint8_t id = printLogHelper(
        [](const void* this__, uint8_t id, bool& current, bool& end, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const MaskMode*>(this__);

            current = this_->m_step != Step::setNum && this_->m_currentMask == id;
            end = id == this_->m_numberOfMasks;
            if (end)
                return {};

            if (this_->m_notifyMask & (1 << id))
                mark = "ntf";

            Time time = this_->m_masks[id];
            if (time == -1_ts)
                return 0_ts;

            return time;
        },
        this);

    if (m_step != Step::setNum) {
        if (m_currentMask >= id && m_currentMask != m_numberOfMasks)
            logOverFlowed = true;
    }
}

void MaskMode::printLog() const {
    gDisplay[0] << "Log ";

    bool unused;
    printLog(unused);
}
