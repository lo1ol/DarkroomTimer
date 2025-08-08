#include "Timer.h"

#include "Hardware.h"

void Timer::tick() {
    m_currentTime = gMillis();
    m_justStopped = false;
    m_justFinished = false;

    if (m_status != RUNNING)
        return;

    if (m_currentTime >= realStopTime()) {
        m_justFinished = true;
        stop();
        return;
    }

    updateAfterLastResume();

    if (gBeeper.state() != Beeper::State::on) {
        auto passedTime = afterResume();
        if (!m_lagPassed && passedTime)
            m_lagPassed = true;
        if (passedTime >= 1000)
            gBeeper.start();
    }
}

void Timer::start(Time time) {
    gBeeper.beep();

    if (time == 0_ts) {
        m_justFinished = true;
        m_justStopped = true;
        return;
    }

    m_lagPassed = false;
    m_leftTime = time.toMillis();
    m_resumeTime = m_currentTime;
    m_status = RUNNING;
    updateAfterLastResume();
    gDigitalWrite(RELAY, true);
}

bool Timer::pause() {
    if (m_status != RUNNING)
        return false;

    gBeeper.stop();
    updateAfterLastResume();
    m_total += afterResume();
    m_leftTime -= afterResume();
    m_status = PAUSED;
    gDigitalWrite(RELAY, false);

    return m_currentTime > (m_resumeTime + gSettings.lagTime.toMillis());
}

void Timer::resume() {
    if (m_status != PAUSED)
        return;

    gBeeper.beep();
    m_lagPassed = false;
    m_resumeTime = m_currentTime;
    m_status = RUNNING;
    gDigitalWrite(RELAY, true);
}

void Timer::stop() {
    gBeeper.stop();
    gDigitalWrite(RELAY, false);

    if (m_status == STOPPED)
        return;

    m_justStopped = true;

    updateAfterLastResume();
    if (m_currentTime >= realStopTime())
        m_total += m_leftTime;
    else
        m_total += afterResume();
    m_status = STOPPED;
    m_leftTime = 0;
}

uint32_t Timer::left() const {
    if (m_status == RUNNING && afterResume())
        return realStopTime() - m_currentTime;

    return m_leftTime;
}

Time Timer::afterLastResume() const {
    return m_afterLastResume;
}

void Timer::updateAfterLastResume() {
    if (uint32_t lastResume = afterResume())
        m_afterLastResume = Time::fromMillis(lastResume);
}

uint32_t Timer::afterResume() const {
    if (m_status != RUNNING)
        return 0;

    if (m_currentTime > realStopTime())
        return m_leftTime;

    uint32_t realAfterResume = m_currentTime - m_resumeTime;

    if (realAfterResume < static_cast<uint16_t>(gSettings.lagTime.toMillis()))
        return 0;

    return realAfterResume - static_cast<uint16_t>(gSettings.lagTime.toMillis());
}

uint32_t Timer::realStopTime() const {
    return m_resumeTime + m_leftTime + gSettings.lagTime.toMillis();
}

Time Timer::total() const {
    return Time{ static_cast<int32_t>(m_total + afterResume()) / 100 };
}

void Timer::resetTotal() {
    m_total = 0;
}

void Timer::reset() {
    stop();
    resetTotal();
    m_afterLastResume = 0_ts;
}

Timer::State Timer::state() const {
    return m_status;
}

bool Timer::lag() const {
    if (m_status != RUNNING)
        return false;

    return afterResume() == 0;
}
