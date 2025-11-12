#pragma once

#include "TestMode.h"

class LinearTestMode final : public TestMode {
public:
    explicit LinearTestMode(SubMode subMode);

protected:
    const __FlashStringHelper* header() const override;

    bool handleStepSet() override;
    void printStep() const override;

    Time getStepTime_(uint8_t step) const override;
    Time getStepTotalTime_(uint8_t step) const override;

    Time m_stepTime;
};
