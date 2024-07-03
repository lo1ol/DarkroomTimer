#include "Timer.h"

#include "DisplayLine.h"
#include "Tools.h"

void Timer::setup() {
    pinMode(m_beepPin, OUTPUT);
    pinMode(m_controlPin, OUTPUT);
    analogWrite(m_beepPin, 0);
    analogWrite(m_controlPin, 0);
}

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

    // beep when we press start btn and every second
    uint32_t passed = afterResume() / 100;
    if ((m_currentTime - m_resumeTime <= 100) || (passed > 5 && !(passed % 10)))
        analogWrite(m_beepPin, gSettings.beepVolume);
    else
        analogWrite(m_beepPin, 0);
}

void Timer::start(Time time) {
    m_leftTime = time.toMillis();
    m_resumeTime = m_currentTime;
    m_status = RUNNING;
    digitalWrite(m_controlPin, HIGH);
}

bool Timer::pause() {
    if (m_status == RUNNING) {
        analogWrite(m_beepPin, 0);
        updateAfterLastResume();
        m_total += afterResume();
        m_leftTime -= afterResume();
        m_status = PAUSED;
        digitalWrite(m_controlPin, LOW);

        return m_currentTime > (m_resumeTime + gSettings.lagTime.toMillis());
    }

    return false;
}

void Timer::resume() {
    if (m_status == PAUSED) {
        m_resumeTime = m_currentTime;
        m_status = RUNNING;
        digitalWrite(m_controlPin, HIGH);
    }
}

void Timer::stop() {
    if (m_status != STOPPED) {
        analogWrite(m_beepPin, 0);
        updateAfterLastResume();
        if (m_currentTime >= realStopTime())
            m_total += m_leftTime;
        else
            m_total += afterResume();
        m_status = STOPPED;
        m_leftTime = 0;
        digitalWrite(m_controlPin, LOW);
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
    return Time::fromMillis(m_total + afterResume());
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
