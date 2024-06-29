#include "Settings.h"

#include <EEPROM.h>

Settings::Settings() {
    int idx = 0;
    uint32_t hash = 0;
#define GET_SETTING(value)    \
    EEPROM.get(idx, (value)); \
    idx += sizeof(value);     \
    hash += value;

    GET_SETTING(lagTime);
    GET_SETTING(beepVolume);
    GET_SETTING(backlight);
    GET_SETTING(autoFinishViewMinutes);

    uint32_t realHash = hash;
    uint32_t storedHash;
    GET_SETTING(storedHash);

    if (realHash != storedHash) {
        lagTime = 0;
        beepVolume = 5;
        backlight = 3 * 5;
        autoFinishViewMinutes = 3;
        updateEEPROM();
        return;
    }
}

void Settings::updateEEPROM() {
    int idx = 0;
    uint32_t hash = 0;
#define PUT_SETTING(value)    \
    EEPROM.put(idx, (value)); \
    idx += sizeof(value);     \
    hash += value;

    PUT_SETTING(lagTime);
    PUT_SETTING(beepVolume);
    PUT_SETTING(backlight);
    PUT_SETTING(autoFinishViewMinutes);
    PUT_SETTING(hash);
}
