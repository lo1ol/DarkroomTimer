#pragma once

#include "AbstractMaskMode.h"

#include "../RelTimeTable.h"

class RelMaskMode final : public AbstractMaskMode {
public:
    explicit RelMaskMode(uint8_t filterNum);

protected:
    void onUpdateTimeTableNums() override;

    void setCurrentMask_(uint8_t filter, uint8_t mask, bool needGuessMask) override;

    bool handleSetCurrentTime() override;
    bool supportNotifyMasks() const override { return false; }

    bool handleEncoderClickAtRun() override;

    Time getStepTime() const override;

    void printHeaderForSetNum() const override;
    void flushTimeTables() override;
    void flashTimeTablesAtRun() override;

private:
    RelTimeTable m_relTimeTable[2];
};
