#include "ModeProcessor.h"

#include <stdint.h>

class LinearTestMode final : public ModeProcessor {
    enum class Step { initTime, stepTime, run, last_ };

public:
    LinearTestMode();
    void process() override;
    void reset() override;

private:
    Step m_step;
    uint8_t m_currentRun;
    uint32_t m_initTime;
    uint32_t m_stepTime;
};
