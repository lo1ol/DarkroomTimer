#pragma once

#include <stdint.h>

struct Settings {
    Settings();
    void updateEEPROM();

    uint16_t lagTime;
    uint8_t beepVolume;
    uint8_t backlight;
    uint8_t autoFinishViewMinutes;
};
