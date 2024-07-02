#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class LinearTestMode final : public ModeProcessor {
    enum class Step { initTime, stepTime, run, last_ };

public:
    LinearTestMode();
    void process() override;
    void reset() override;
    void printLog() const override;
    void switchMode() override;

private:
    Step m_step;
    uint8_t m_currentRun;
    Time m_initTime;
    Time m_stepTime;
};
