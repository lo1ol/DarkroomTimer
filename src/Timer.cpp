#include "Timer.h"

#include "Tools.h"

void Timer::setup() {
    pinMode(m_beepPin, OUTPUT);
    pinMode(m_controlPin, OUTPUT);
    analogWrite(m_beepPin, 0);
    analogWrite(m_controlPin, 0);
}

void Timer::tick() {
    m_currentTime = millis();
    if (m_status != RUNNING)
        return;

    if (m_currentTime >= realStopTime()) {
        stop();
        return;
    }

    // beep when we press start btn
    if ((m_leftTime - m_stopTime + m_currentTime) < 100) {
        analogWrite(m_beepPin, gSettings.beepVolume);
    } else if ((m_leftTime - left() < 500) || ((m_leftTime - left()) / 100) % 10) {
        analogWrite(m_beepPin, 0);
    } else {
        // beep every second
        analogWrite(m_beepPin, gSettings.beepVolume);
    }

    updateAfterLastResume();
}

void Timer::start(uint32_t ms) {
    m_leftTime = ms;
    m_stopTime = m_currentTime + ms;
    m_status = RUNNING;
    digitalWrite(m_controlPin, HIGH);
}

void Timer::pause() {
    if (m_status == RUNNING) {
        analogWrite(m_beepPin, 0);
        updateAfterLastResume();
        m_total += afterResume();
        m_leftTime -= afterResume();
        m_status = PAUSED;
        digitalWrite(m_controlPin, LOW);
    }
}

void Timer::resume() {
    if (m_status == PAUSED) {
        m_stopTime = m_currentTime + m_leftTime;
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

uint32_t Timer::afterLastResume() const {
    return m_afterLastResume;
}

void Timer::updateAfterLastResume() {
    if (uint32_t lastResume = afterResume())
        m_afterLastResume = lastResume;
}

void Timer::resetAfterLastResume() {
    m_afterLastResume = 0;
}

uint32_t Timer::afterResume() const {
    if (m_status != RUNNING)
        return 0;

    uint32_t realAfterResume = m_currentTime + m_leftTime - m_stopTime;

    if (realAfterResume < gSettings.lagTime)
        return 0;

    return realAfterResume - gSettings.lagTime;
}

void Timer::printFormatedState(const char* additionalInfo) const {
    char str[MAX_SYMS_PER_LINE + 1] = "";
    switch (m_status) {
    case RUNNING:
        if (afterResume() == 0) {
            concat(str, "Lag");
        } else {
            concatTime(str, left());
        }
        break;
    case PAUSED:
        concatTime(str, left());
        concat(str, " PAUSE");
    case STOPPED:
        break;
    }

    concatBack(str, additionalInfo);
    printFormatedLine(str, 1);
}

uint32_t Timer::realStopTime() const {
    return m_stopTime + gSettings.lagTime;
}

uint32_t Timer::total() const {
    return m_total + afterResume();
}

void Timer::resetTotal() {
    m_total = 0;
}

Timer::State Timer::state() const {
    return m_status;
}
