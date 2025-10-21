#pragma once

#include <stdint.h>

#include "common/Communication/ComData.h"

struct Settings {
    void updateEEPROM();
    [[nodiscard]] static Settings load();

    Communication::BaseTime bwBaseTime;
    Communication::BaseTime colorBaseTime;
};

constexpr Settings kDefaultSettings{ .bwBaseTime = { .baseLogD = 400, .baseTime = 40_s },
                                     .colorBaseTime = { .baseLogD = 400, .baseTime = 20_s } };
