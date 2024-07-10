#pragma once

#include <stdint.h>

#include "Melody.h"

class Beeper {
public:
    Beeper(uint8_t pin) : m_pin(pin) {}

    enum class State { off, on, single, alarm };

    void beep();
    void alarm(const char* notification = nullptr);

    bool blocking() const;

    // start beep every second
    void start(bool silentStart = false);
    void stop();

    void setup();
    void tick();

    void setMelody(Melody::Name);

private:
    void processAlarm();
    void processPin() const;

    bool m_pinState = false;
    State m_state = State::off;
    uint8_t m_pin;
    uint32_t m_timer;
    const char* m_notification;
    Melody* m_melody;
};
