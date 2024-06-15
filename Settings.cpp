#include "Settings.h"

#include <EEPROM.h>

namespace {
constexpr uint32_t kSettingsWasInitedFlag = 0xABCDEFFE;
} // namespace

Settings::Settings() {
    int idx = 0;
#define GET_SETTING(value)    \
    EEPROM.get(idx, (value)); \
    idx += sizeof(value);

    uint32_t checkInitFlag;
    GET_SETTING(checkInitFlag);
    if (checkInitFlag != kSettingsWasInitedFlag) {
        lagTime = 0;
        beepVolume = 10;
        updateEEPROM();
        return;
    }

    GET_SETTING(lagTime);
    GET_SETTING(beepVolume);
}

void Settings::updateEEPROM() {
    int idx = 0;
#define PUT_SETTING(value)    \
    EEPROM.put(idx, (value)); \
    idx += sizeof(value);

    PUT_SETTING(kSettingsWasInitedFlag);
    PUT_SETTING(lagTime);
    PUT_SETTING(beepVolume);
}
