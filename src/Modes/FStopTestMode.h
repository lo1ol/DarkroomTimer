#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class FStopTestMode final : public ModeProcessor {
    enum class Step { baseTime, initTime, fstopSet, run, last_ };

public:
    FStopTestMode(bool splitGrade);
    void process() override;
    void reset() override;
    void switchMode() override;

    const char* preview() const override;

private:
    void printTimes() const;
    Time getPrintTime() const;
    Time getStepTotalTime(uint8_t step) const;

    bool kSplit;
    Step m_step;
    uint8_t m_currentRun;
    uint8_t m_FStopPartId;
    Time m_baseTime;
    Time m_initTime;
};
