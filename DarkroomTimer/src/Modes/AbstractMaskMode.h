#pragma once

#include "../ModeProcessor.h"

#include <stdint.h>

#include "../Time.h"

class AbstractMaskMode : public ModeProcessor {
protected:
    enum class Step { setNum, setMasks, run, last_ };

public:
    explicit AbstractMaskMode(uint8_t filterNum);
    void process() override;
    void reset() override;
    void switchMode() override;

    void repaint() override;

private:
    void processRun();
    void processSetMasks();

    void setCurrentMask(uint8_t filter, uint8_t mask, bool needGuessMask = false);
    void moveCurrentMask(int8_t dir, bool needGuessMask = false);

protected:
    void printTimeExplicit() const;

    virtual void onUpdateTimeTableNums() = 0;

    virtual void setCurrentMask_(uint8_t filter, uint8_t mask, bool needGuessMask) = 0;

    virtual bool handleSetCurrentTime() = 0;
    virtual bool supportNotifyMasks() const = 0;

    virtual bool handleEncoderClickAtRun() = 0;

    virtual Time getStepTime() const = 0;

    virtual void printHeaderForSetNum() const = 0;
    virtual void flushTimeTables() = 0;
    virtual void flashTimeTablesAtRun() = 0;

    Step m_step = Step::setNum;
    uint8_t m_numberOfMasks[2];
    int8_t m_currentMask = 0;
    uint8_t m_currentFilter = 0;
    uint32_t m_notifyMask = 0;

    const uint8_t kFilterNum;
    const uint8_t kMaxMasksNumber;
};
