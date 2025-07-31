#include "Timer.h"

#include "Tools.h"

void Timer::tick() {
    m_currentTime = gMillis();
    m_justStopped = false;
    if (m_status != RUNNING)
        return;

    if (m_currentTime >= realStopTime()) {
        m_justStopped = true;
        stop();
        return;
    }

    updateAfterLastResume();

    if (!m_lagPassed && afterResume() / 100) {
        m_lagPassed = true;
        gBeeper.start(true);
    }
}

void Timer::start(Time time) {
    gBeeper.beep();

    if (time == 0_ts) {
        m_justStopped = true;
        return;
    }

    m_lagPassed = false;
    m_leftTime = time.toMillis();
    m_resumeTime = m_currentTime;
    m_status = RUNNING;
    updateAfterLastResume();
    gDigitalWrite(RELAY, HIGH);
}

bool Timer::pause() {
    if (m_status != RUNNING)
        return false;

    gBeeper.stop();
    updateAfterLastResume();
    m_total += afterResume();
    m_leftTime -= afterResume();
    m_status = PAUSED;
    gDigitalWrite(RELAY, LOW);

    return m_currentTime > (m_resumeTime + gSettings.lagTime.toMillis());
}

void Timer::resume() {
    if (m_status != PAUSED)
        return;

    gBeeper.beep();
    m_lagPassed = false;
    m_resumeTime = m_currentTime;
    m_status = RUNNING;
    gDigitalWrite(RELAY, HIGH);
}

void Timer::stop() {
    if (m_status == STOPPED)
        return;

    gBeeper.stop();
    updateAfterLastResume();
    if (m_currentTime >= realStopTime())
        m_total += m_leftTime;
    else
        m_total += afterResume();
    m_status = STOPPED;
    m_leftTime = 0;
    gDigitalWrite(RELAY, LOW);
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
    int32_t total = (m_total + afterResume()) / 100;
    if (total > INT16_MAX)
        return Time{ INT16_MAX };

    return Time{ static_cast<int16_t>(total) };
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
