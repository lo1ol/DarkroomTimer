#include "ModeProcessor.h"

#include <stddef.h>

class LinearTestMode final : public ModeProcessor {
    enum class Step { initTime, stepTime, run, last_ };

public:
    LinearTestMode();
    void process() override;

private:
    Step m_step;
    size_t m_currentRun;
    unsigned long m_initTime;
    unsigned long m_stepTime;
};
