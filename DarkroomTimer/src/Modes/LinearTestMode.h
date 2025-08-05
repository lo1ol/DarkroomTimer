#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"
#include "../TimeTable.h"

class LinearTestMode final : public ModeProcessor {
    enum class Step { baseTime, initTime, stepTime, run, last_ };

public:
    enum SubMode {
        Generic,
        SplitGrade,
        Local,
    };

    explicit LinearTestMode(SubMode subMode);
    void process() override;
    void reset() override;
    void switchMode() override;

    const char* preview() const;

    void repaint() override;

private:
    void setTimeTable();
    Time getPrintTime() const;
    Time getTotalTime(uint8_t id) const;

    const SubMode kSubMode;
    Step m_step;
    uint8_t m_currentRun;
    Time m_baseTime;
    Time m_initTime;
    Time m_stepTime;

    TimeTable m_timeTable;
};
