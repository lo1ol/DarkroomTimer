#include "ModeProcessor.h"

#include <stddef.h>

class FStopTestMode final : public ModeProcessor {
    enum class Step { initTime, fstopSet, run, log, last_ };

public:
    FStopTestMode();
    void process() override;

private:
    Step m_step;
    size_t m_currentRun;
};
