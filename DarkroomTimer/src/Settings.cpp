#include "Settings.h"

#include <CRC32.h>
#include <EEPROM.h>

#include "Config.h"

Settings::Settings() {
    int idx = 0;
    CRC32 crc32;
#define GET_SETTING(value)    \
    EEPROM.get(idx, (value)); \
    idx += sizeof(value);     \
    crc32.update(value);

    GET_SETTING(lagTime);
    GET_SETTING(beepVolume);
    GET_SETTING(backlight);
    GET_SETTING(autoFinishViewMinutes);
    GET_SETTING(startWithSettings);
    GET_SETTING(melody);

    uint32_t hash = crc32.finalize();
    uint32_t storedHash;
    GET_SETTING(storedHash);
#undef GET_SETTING

    if (hash != storedHash) {
        lagTime = 0_ts;
        beepVolume = 3 * 3;
        backlight = (3 * MAX_BACKLIGHT) / 10;
        autoFinishViewMinutes = 3;
        startWithSettings = false;
        melody = Melody::nice;
        updateEEPROM();
        return;
    }
}

void Settings::updateEEPROM() {
    int idx = 0;
    CRC32 crc32;
#define PUT_SETTING(value)    \
    EEPROM.put(idx, (value)); \
    idx += sizeof(value);     \
    crc32.update(value);

    PUT_SETTING(lagTime);
    PUT_SETTING(beepVolume);
    PUT_SETTING(backlight);
    PUT_SETTING(autoFinishViewMinutes);
    PUT_SETTING(startWithSettings);
    PUT_SETTING(melody);
    PUT_SETTING(crc32.finalize());
#undef PUT_SETTING
}
