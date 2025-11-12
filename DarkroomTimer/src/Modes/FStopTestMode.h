#pragma once

#include "TestMode.h"

class FStopTestMode final : public TestMode {
public:
    explicit FStopTestMode(SubMode subMode);

protected:
    const __FlashStringHelper* header() const override;

    bool handleStepSet() override;
    void printStep() const override;

    Time getStepTime_(uint8_t step) const override;
    Time getStepTotalTime_(uint8_t step) const override;

    uint8_t m_FStopPartId;
};
