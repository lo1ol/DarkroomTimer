#pragma once

#include <stdint.h>

#include "Config.h"
#include "Melody.h"
#include "Time.h"

constexpr Time kMinLagTime = 0_s;
constexpr Time kMaxLagTime = 2_s;
constexpr uint8_t kMinBeepVolume = 1;
constexpr uint8_t kMaxBeepVolume = 10;
constexpr uint8_t kMinBacklight = 1;
constexpr uint8_t kMaxBacklight = 10;
constexpr uint8_t kMaxAutoFinishViewMinutes = 10;

struct Settings {
    Settings& operator=(const Settings&) = default;

    void updateEEPROM();
    [[nodiscard]] static Settings load();

    Time lagTime;
    uint8_t beepVolume;
    uint8_t backlight;
    uint8_t autoFinishViewMinutes;
    bool startWithSettings;
    Melody::Name melody;
};

constexpr Settings kDefaultSettings{
    .lagTime = 0_ts,
    .beepVolume = 3,
    .backlight = 5,
    .autoFinishViewMinutes = 3,
    .startWithSettings = false,
    .melody = Melody::nice,
};
