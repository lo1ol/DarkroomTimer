#pragma once

#include "common/Communication/ComData.h"

class Lightmeter {
public:
    enum class Mode {
        BW,
        Color,
    };

    void switchMode(Mode);
    Mode getMode() const { return m_mode; }

    Communication::LightDesc getSmoothLightDesc();

private:
    Communication::LightDesc getSingleColorMeasure();

    static constexpr const uint8_t kMeasureAverageCnt = 5;

    Communication::LightDesc m_measures[kMeasureAverageCnt];
    uint8_t m_measureNum = 0;
    Communication::LightDesc m_measureSum;
    Mode m_mode = Mode::Color;
};
