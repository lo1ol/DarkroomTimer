// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <assert.h>
#include <stdint.h>

namespace Communication {

struct LightDesc {
    int16_t lm;
    int16_t y;
    int16_t m;
    int16_t c;
    bool abs = true;

    ColorDesc operator-(const LightDesc& o) {
        ColorDesc res;
        res.lm = lm - o.lm;
        res.y = lm - o.y;
        res.m = lm - o.m;
        res.c = lm - o.c;
        res.abs = false;
        return res;
    }

    void operator-=(const LightDesc& o) { *this = *this - o; }
};

// Lightmeter commands
enum class LmCmd : uint8_t {
    askStart, // lightmeter asks for start
    last_
};

// Main commands
enum class MainCmd : uint8_t {
    switchBW,
    switchVC,
    switchColorNeg,
    sendLightInfo, // ask to send ColorInfo
    end,           // ask for end
};

} // namespace Communication

