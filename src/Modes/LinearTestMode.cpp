#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode() {
    m_initTime = 80_ts;
    m_stepTime = 20_ts;
    m_step = Step::initTime;
    m_view = gSettings.logViewInTests ? RunView::log : RunView::common;
    m_currentRun = 0;
}

void LinearTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    m_currentRun = 0;
    gTimer.reset();
}

void LinearTestMode::process() {
    switch (m_step) {
    case Step::initTime:
        gDisplay[0] << "Linear tests";
        getTime(m_initTime);
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::stepTime:
        gDisplay[0] << "Linear tests";
        getTime(m_stepTime);
        gDisplay[1] << "Step t:" << m_stepTime;
        return;
    case Step::run:
        break;
    }

    switch (m_view) {
    case RunView::common:
        gDisplay[0] << "L Test#" << m_currentRun + 1 << " T:" << gTimer.total();

        if (gTimer.state() == Timer::RUNNING) {
            gTimer.printFormatedState();
            return;
        }

        gDisplay[1] << getPrintTime();
        break;
    case RunView::log: {
        gDisplay[0] << "L Run ";

        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = RunView::common;
    } break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click())
        gTimer.start(getPrintTime());

    if (gTimer.stopped())
        ++m_currentRun;
}

Time LinearTestMode::getPrintTime() const {
    if (m_currentRun == 0)
        return m_initTime;

    return m_stepTime;
}

void LinearTestMode::reset() {
    m_currentRun = 0;
}

void LinearTestMode::switchView() {
    m_view = ADD_TO_ENUM(RunView, m_view, 1);
}

bool LinearTestMode::canSwitchView() const {
    if (m_step != Step::run)
        return false;

    gDisplay[0] << "L Run ";
    bool overFlow = false;
    printLog(overFlow);
    gDisplay.reset();
    return !overFlow;
}

void LinearTestMode::printLog(bool& logOverFlowed) const {
    uint8_t id = printLogHelper(
        [](const void* this__, uint8_t id, bool& current, bool& end) -> Time {
            auto this_ = reinterpret_cast<const LinearTestMode*>(this__);
            current = this_->m_step == Step::run && this_->m_currentRun == id;

            return { this_->m_initTime + this_->m_stepTime * id };
        },
        this);

    if (m_step == Step::run && m_currentRun >= id)
        logOverFlowed = true;
}

void LinearTestMode::printLog() const {
    gDisplay[0] << "L Log ";

    bool unused;
    printLog(unused);
}
