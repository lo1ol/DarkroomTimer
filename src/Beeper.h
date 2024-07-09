#pragma once

#include <stdint.h>

class Beeper {
public:
    Beeper(uint8_t pin) : m_pin(pin) {}

    enum class State { off, on, single, alarm };

    void beep();
    void alarm(const char* notification);

    bool blocking() const;

    // start beep every second
    void start(bool silentStart = false);
    void stop();

    void setup();
    void tick();

private:
    void processAlarm();
    void processPin() const;

    bool m_pinState = false;
    uint8_t m_melodyPhase;
    State m_state = State::off;
    uint8_t m_pin;
    uint32_t m_timer;
    const char* m_notification;
};
