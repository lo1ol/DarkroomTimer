#pragma once

#include <stdint.h>

#include "Time.h"

class Timer {
public:
    enum State { STOPPED, RUNNING, PAUSED };

    Timer() = default;

    void tick();
    void start(Time ms);
    // info was it really paused
    bool pause();
    void resume();
    void stop();
    [[nodiscard]] uint32_t left() const;
    [[nodiscard]] bool lag() const;

    [[nodiscard]] bool justFinished() const { return m_justFinished; }
    [[nodiscard]] bool justStopped() const { return m_justStopped; }

    [[nodiscard]] Time afterLastResume() const;

    [[nodiscard]] Time total() const;
    void resetTotal();

    void reset();

    [[nodiscard]] State state() const;

private:
    void updateAfterLastResume();
    [[nodiscard]] uint32_t afterResume() const;
    [[nodiscard]] uint32_t realStopTime() const;

    bool m_justFinished = false;
    bool m_justStopped = false;
    bool m_lagPassed = false;

    State m_status = STOPPED;

    Time m_afterLastResume;

    uint32_t m_currentTime = 0;
    uint32_t m_resumeTime = 0;
    uint32_t m_leftTime = 0;
    uint32_t m_total = 0;
};
