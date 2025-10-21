// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <stdint.h>

#include "../Time.h"

namespace Communication {

struct LightDesc {
    int16_t logD = 0;
    int16_t logBlue = 0;
    int16_t logGreen = 0;

    LightDesc operator-(const LightDesc& o) const {
        LightDesc res;
        res.logD = logD - o.logD;
        res.logGreen = logGreen - o.logGreen;
        res.logBlue = logBlue - o.logBlue;
        return res;
    }

    LightDesc operator+(const LightDesc& o) const {
        LightDesc res;
        res.logD = logD + o.logD;
        res.logGreen = logGreen + o.logGreen;
        res.logBlue = logBlue + o.logBlue;
        return res;
    }

    LightDesc operator/(uint8_t val) const {
        LightDesc res;
        res.logD = logD / val;
        res.logGreen = logGreen / val;
        res.logBlue = logBlue / val;
        return res;
    }

    void operator-=(const LightDesc& o) { *this = *this - o; }
    void operator+=(const LightDesc& o) { *this = *this + o; }
};

struct BaseTime {
    uint16_t baseLogD;
    Time baseTime;
};

struct LightMeasure {
    LightDesc lightDesc;
    Time suggestedTime;
    bool abs;
};

// Lightmeter commands
enum class LmCmd : uint8_t {
    askStart, // lightmeter asks for start
};

// Main commands
enum class MainCmd : uint8_t {
    switchBW,         // ask to switch to BW mode
    switchColor,      // ask to swutch to Color mode
    sendLightMeasure, // ask to send LightMeasure
    sendBaseTime,     // ask to send BaseTime
    setBaseTime,      // ask to set new BaseTime for current Mode
    end,              // ask for end
};

} // namespace Communication

