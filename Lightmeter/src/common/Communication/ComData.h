// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <assert.h>
#include <stdint.h>

namespace Communication {

struct ColorDesc {
    int16_t lm;
    int16_t y;
    int16_t m;
    int16_t c;
    bool abs = true;

    ColorDesc operator-(const ColorDesc& o) {
        ColorDesc res;
        res.lm = lm - o.lm;
        res.y = lm - o.y;
        res.m = lm - o.m;
        res.c = lm - o.c;
        res.abs = false;
        return res;
    }

    void operator-=(const ColorDesc& o) { *this = *this - o; }
};

static_assert(sizeof(ColorDesc) == 9);

// Lightmeter commands
enum class LmCmd : uint8_t {
    askStart, // lightmeter asks for start
    last_
};

// Main commands
enum class MainCmd : uint8_t {
    sendColorInfo, // ask to send ColorInfo
    end,           // ask for end
};

} // namespace Communication

