#include "ModeProcessor.h"

#include <stdint.h>

class FStopTestMode final : public ModeProcessor {
    enum class Step { initTime, fstopSet, run, log, last_ };

public:
    FStopTestMode();
    void process() override;
    void reset() override;

private:
    Step m_step;
    uint8_t m_currentRun;
};
