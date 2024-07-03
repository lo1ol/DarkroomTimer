#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class LinearTestMode final : public ModeProcessor {
    enum class Step { initTime, stepTime, run, last_ };

public:
    LinearTestMode();
    void process() override;
    void reset() override;
    void printLog() override;
    void switchMode() override;
    bool supportPrintInLog() const override { return true; }

private:
    Time getPrintTime() const;
    void processRun();

    Step m_step;
    uint8_t m_currentRun;
    Time m_initTime;
    Time m_stepTime;
};
