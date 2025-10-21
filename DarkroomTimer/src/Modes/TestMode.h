#pragma once

#include "../ModeProcessor.h"

#include <stdint.h>

#include "../TimeTable.h"
#include "../common/Time.h"

class TestMode : public ModeProcessor {
protected:
    enum class Step { setLocalized, preflashTime, initTime, setStep, run, last_ };

public:
    enum SubMode {
        Generic,
        SplitGrade,
        Local,
        Expert,
    };

    explicit TestMode(SubMode subMode);
    void process() override;
    void reset() override;
    void switchMode() override;

    void repaint() override;

protected:
    virtual const char* header() const = 0;

    virtual bool handleStepSet() = 0;
    virtual void printStep() const = 0;

    // returns raw time. Without preflash time
    // it doesn't take into account localization
    virtual Time getStepTime_(uint8_t step) const = 0;
    virtual Time getStepTotalTime_(uint8_t step) const = 0;

private:
    // returns time taking into account preflash time
    Time getPrintTime() const;
    Time getStepTotalTime(uint8_t step) const;
    bool hasPreflashTime() const;

    void setTimeTable();

    Step m_step;
    TimeTable m_timeTable;
    uint8_t m_currentRun;
    bool m_localized;

protected:
    const SubMode kSubMode;
    Time m_preflashTime;
    Time m_initTime;
};
