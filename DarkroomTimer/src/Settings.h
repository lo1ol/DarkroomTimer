#pragma once

#include <stdint.h>

#include "Config.h"
#include "Melody.h"
#include "Time.h"

enum class StartWith : uint8_t {
    Modes,
    Settings,
    last_,
};

constexpr Time kMinLagTime = 0_s;
constexpr Time kMaxLagTime = 2_s;
constexpr uint8_t kMinBeepVolume = 1;
constexpr uint8_t kMaxBeepVolume = 10;
constexpr uint8_t kMinBacklight = 1;
constexpr uint8_t kMaxBacklight = 10;
constexpr uint8_t kMaxAutoFinishViewMinutes = 10;

struct Settings {
    Settings& operator=(const Settings&) = default;
    bool operator==(const Settings&) const;

    void updateEEPROM();
    [[nodiscard]] static Settings load();

    Time lagTime;
    uint8_t beepVolume;
    uint8_t backlight;
    uint8_t autoFinishViewMinutes;
    enum StartWith startWith;
    Melody::Name melody;
};

constexpr Settings kDefaultSettings{
    .lagTime = 0_ts,
    .beepVolume = 3,
    .backlight = 5,
    .autoFinishViewMinutes = 3,
    .startWith = StartWith::Modes,
    .melody = Melody::nice,
};
