#pragma once

#include <stdint.h>

#include "Melody.h"
#include "Time.h"

struct Settings {
    Settings();
    void updateEEPROM();

    Time lagTime;
    uint8_t beepVolume;
    uint8_t backlight;
    uint8_t autoFinishViewMinutes;
    bool startWithSettings;
    Melody::Name melody;
};
