#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode(bool splitGrade) : kSplit(splitGrade) {
    m_baseTime = 2_s;
    m_initTime = 8_s;
    m_stepTime = 2_s;
    m_step = kSplit ? Step::baseTime : Step::initTime;
    m_currentRun = kSplit ? 0 : 1;

    repaint();
}

void LinearTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && !kSplit)
        m_step = Step::initTime;

    m_currentRun = kSplit ? 0 : 1;
    gTimer.reset();

    repaint();
}

void LinearTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        if (getTime(m_baseTime))
            repaint();
        return;
    case Step::initTime:
        if (getTime(m_initTime))
            repaint();
        return;
    case Step::stepTime:
        if (getTime(m_stepTime))
            repaint();
        return;
    case Step::run:
        break;
    }

    if (gTimer.state() == Timer::RUNNING)
        repaint();

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getTotalTime(m_currentRun) != kBadTime)
        gTimer.start(getPrintTime());

    if (gTimer.stopped()) {
        if (m_currentRun == 0) {
            gTimer.reset();
            gBeeper.alarm("Change filter");
        }
        ++m_currentRun;
        repaint();
    }
}

void LinearTestMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::baseTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Base t:" << m_baseTime;
        return;
    case Step::initTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::stepTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Step t:" << m_stepTime;
        return;
    case Step::run:
        gDisplay[0] << "Run ";
        printTimes();
        break;
    }
}

Time LinearTestMode::getPrintTime() const {
    if (m_currentRun == 0)
        return m_baseTime;

    if (m_currentRun == 1)
        return m_initTime;

    return m_stepTime;
}

Time LinearTestMode::getTotalTime(uint8_t id) const {
    if (id == 0)
        return m_baseTime;

    int32_t res = static_cast<int16_t>(m_stepTime);
    res *= id - 1;
    res += static_cast<int16_t>(m_initTime);

    return Time{ res };
}
void LinearTestMode::reset() {
    m_currentRun = kSplit ? 0 : 1;
}

void LinearTestMode::printTimes() const {
    printTimeHelper(
        [](const void* this__, uint8_t id, bool& current, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const LinearTestMode*>(this__);

            if (!this_->kSplit)
                ++id;

            current = this_->m_step == Step::run && this_->m_currentRun == id;

            if (id == 0)
                mark = "ntf";

            return this_->getTotalTime(id);
        },
        this);
}

const char* LinearTestMode::preview() const {
    if (kSplit)
        return "Splt linear test";
    return "Linear test";
}
