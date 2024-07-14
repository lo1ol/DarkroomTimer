#include "Timer.h"

#include "DisplayLine.h"
#include "Tools.h"

void Timer::tick() {
    m_currentTime = millis();
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
    digitalWrite(RELAY, HIGH);
}

bool Timer::pause() {
    if (m_status == RUNNING) {
        gBeeper.stop();
        updateAfterLastResume();
        m_total += afterResume();
        m_leftTime -= afterResume();
        m_status = PAUSED;
        digitalWrite(RELAY, LOW);

        return m_currentTime > (m_resumeTime + gSettings.lagTime.toMillis());
    }

    return false;
}

void Timer::resume() {
    if (m_status == PAUSED) {
        gBeeper.beep();
        m_lagPassed = false;
        m_resumeTime = m_currentTime;
        m_status = RUNNING;
        digitalWrite(RELAY, HIGH);
    }
}

void Timer::stop() {
    if (m_status != STOPPED) {
        gBeeper.stop();
        updateAfterLastResume();
        if (m_currentTime >= realStopTime())
            m_total += m_leftTime;
        else
            m_total += afterResume();
        m_status = STOPPED;
        m_leftTime = 0;
        digitalWrite(RELAY, LOW);
    }
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

void Timer::resetAfterLastResume() {
    m_afterLastResume = 0_ts;
}

uint32_t Timer::afterResume() const {
    if (m_status != RUNNING)
        return 0;

    uint32_t realAfterResume = m_currentTime - m_resumeTime;

    if (realAfterResume < static_cast<uint16_t>(gSettings.lagTime.toMillis()))
        return 0;

    return realAfterResume - static_cast<uint16_t>(gSettings.lagTime.toMillis());
}

void Timer::printFormatedState() const {
    switch (m_status) {
    case RUNNING:
        if (afterResume() == 0) {
            gDisplay[1] << "Lag";
        } else {
            gDisplay[1] << Time::fromMillis(left());
        }
        break;
    case PAUSED:
        gDisplay[1] << Time::fromMillis(left()) << " PAUSE";
    case STOPPED:
        break;
    }
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
    resetAfterLastResume();
}

Timer::State Timer::state() const {
    return m_status;
}

bool Timer::lag() const {
    if (m_status != RUNNING)
        return false;

    return afterResume() == 0;
}
