#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class FStopTestMode final : public ModeProcessor {
    enum class Step { initTime, fstopSet, run, last_ };

public:
    FStopTestMode();
    void process() override;
    void reset() override;
    void printLog() const override;

private:
    Step m_step;
    uint8_t m_currentRun;
    uint8_t m_FStopPartId;
    Time m_initTime;
};
