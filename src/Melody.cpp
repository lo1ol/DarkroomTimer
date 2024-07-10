#include "Melody.h"

#include <Arduino.h>

constexpr uint8_t kNiceMelodySwitchTimes[] = { 25, 25, 12, 12, 6, 6, 40, 80 };

class NiceMelody : public Melody {
public:
    void init() override {
        m_melodyPhase = 0;
        m_timer = millis() + kNiceMelodySwitchTimes[0] * 10;
    }

    bool tick() override {
        uint32_t currentTime = millis();

        if (m_timer <= currentTime) {
            ++m_melodyPhase;

            if (end())
                return false;

            m_timer = currentTime + kNiceMelodySwitchTimes[m_melodyPhase] * 10;
        }

        return !(m_melodyPhase & 1);
    }

    bool end() const override {
        return m_melodyPhase >= sizeof(kNiceMelodySwitchTimes) / sizeof(kNiceMelodySwitchTimes[0]);
    }

private:
    uint8_t m_melodyPhase;
    uint32_t m_timer;
};

class AlarmMelody : public Melody {
public:
    void init() override {
        m_melodyPhase = 0;
        m_timer = millis() + 60;
    }

    bool tick() override {
        uint32_t currentTime = millis();

        if (m_timer < currentTime) {
            ++m_melodyPhase;
            if (m_melodyPhase % 20 == 19)
                m_timer = currentTime + 200;
            else
                m_timer = currentTime + 50;
        }

        return !(m_melodyPhase & 1);
    }

    bool end() const override { return m_melodyPhase >= 20; }

private:
    uint8_t m_melodyPhase;
    uint32_t m_timer;
};

Melody* Melody::getMelody(Name) {
    return new AlarmMelody;
}
