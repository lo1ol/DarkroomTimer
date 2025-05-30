#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class FStopTestMode final : public ModeProcessor {
    enum class Step { baseTime, initTime, fstopSet, run, last_ };

public:
    enum SubMode {
        Generic,
        SplitGrade,
        Local,
    };

    FStopTestMode(SubMode subMode);
    void process() override;
    void reset() override;
    void switchMode() override;

    const char* preview() const;

    void repaint() const override;

private:
    void setTimeTable() const;
    Time getPrintTime() const;
    Time getStepTotalTime(uint8_t step) const;

    const SubMode kSubMode;
    Step m_step;
    uint8_t m_currentRun;
    uint8_t m_FStopPartId;
    Time m_baseTime;
    Time m_initTime;
};
