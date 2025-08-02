#pragma once

#include <stdint.h>

#include "Config.h"
#include "Melody.h"
#include "Time.h"

struct Settings {
    Settings& operator=(const Settings&) = default;

    void updateEEPROM();
    static Settings load();

    Time lagTime;
    uint8_t beepVolume;
    uint8_t backlight;
    uint8_t autoFinishViewMinutes;
    bool startWithSettings;
    Melody::Name melody;
};

constexpr Settings kDefaultSettings{
    .lagTime = 0_ts,
    .beepVolume = MIN_BEEP_VOLUME + 3 * BEEP_VOLUME_STEP,
    .backlight = 3 * MAX_BACKLIGHT,
    .autoFinishViewMinutes = 3,
    .startWithSettings = false,
    .melody = Melody::nice,
};
