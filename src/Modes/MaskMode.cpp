#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    m_numberOfMasks = 3;
    memset(m_masks, 0, sizeof(m_masks));
    m_masks[0] = 8_s;
    m_step = Step::setNum;
    m_currentMask = 0;
    m_notifyMask = 0;

    for (uint8_t i = 1; i != kMasksMaxNumber; ++i)
        m_masks[i] = kBadTime;

    repaint();
}

void MaskMode::switchMode() {
    gTimer.reset();

    if (m_step != Step::setMasks) {
        m_step = ADD_TO_ENUM(Step, m_step, 1);

        m_currentMask = 0;

        m_notifyMask &= ~((~0) << m_numberOfMasks);
        for (uint8_t i = m_numberOfMasks; i != kMasksMaxNumber; ++i)
            m_masks[i] = kBadTime;

        repaint();
        return;
    }

    if (m_currentMask + 1 == m_numberOfMasks) {
        m_step = Step::run;
        m_currentMask = 0;
        repaint();
        return;
    }

    ++m_currentMask;

    if (m_currentMask == 0 || m_masks[m_currentMask] != kBadTime) {
        repaint();
        return;
    }

    // let's guess unknown masks
    m_masks[m_currentMask] = m_masks[m_currentMask - 1];
    repaint();
}

void MaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        if (getInt(m_numberOfMasks, 2, kMasksMaxNumber))
            repaint();
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

    if (getTime(m_masks[m_currentMask])) {
        repaint();
        gDisplay.resetBlink();
    }
}

void MaskMode::processRun() {
    if (gTimer.state() == Timer::RUNNING)
        repaint();

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentMask < m_numberOfMasks)
        gTimer.start(m_masks[m_currentMask]);

    if (gTimer.stopped()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm("Notification");
        ++m_currentMask;
        repaint();
    }
}

void MaskMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::setNum:
        gDisplay[0] << preview();
        gDisplay[1] << "Mask num: " << m_numberOfMasks;
        return;
    case Step::setMasks:
        gDisplay[0] << "Set ";
        printTimes();
        return;
    case Step::run:
        gDisplay[0] << "Run ";
        printTimes();
        if (m_currentMask == m_numberOfMasks)
            gDisplay[1] >> " Finish";
        return;
    }
}

void MaskMode::reset() {
    m_currentMask = 0;
}

void MaskMode::printTimes() const {
    printTimeHelper(
        [](const void* this__, uint8_t id, bool& current, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const MaskMode*>(this__);

            current = this_->m_step != Step::setNum && this_->m_currentMask == id;
            if (id == this_->m_numberOfMasks)
                return kBadTime;

            if (this_->m_notifyMask & (1 << id))
                mark = "ntf";

            if (this_->m_masks[id] == kBadTime)
                return 0_ts;

            return this_->m_masks[id];
        },
        this);
}
