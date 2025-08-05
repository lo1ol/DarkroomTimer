#include "Settings.h"

#include <CRC32.h>
#include <EEPROM.h>

#include "Config.h"

Settings Settings::load() {
    Settings res;
    int idx = 0;
    CRC32 crc32;
#define GET_SETTING(value)    \
    EEPROM.get(idx, (value)); \
    idx += sizeof(value);     \
    crc32.update(value);

    GET_SETTING(res.lagTime);
    GET_SETTING(res.beepVolume);
    GET_SETTING(res.backlight);
    GET_SETTING(res.autoFinishViewMinutes);
    GET_SETTING(res.startWithSettings);
    GET_SETTING(res.melody);

    uint32_t hash = crc32.finalize();
    uint32_t storedHash;
    GET_SETTING(storedHash);
#undef GET_SETTING

    bool badSettings = false;
    badSettings |= hash != storedHash;
    badSettings |= res.lagTime < kMinLagTime;
    badSettings |= res.lagTime > kMaxLagTime;
    badSettings |= res.beepVolume < kMinBeepVolume;
    badSettings |= res.beepVolume > kMaxBeepVolume;
    badSettings |= res.backlight < kMinBacklight;
    badSettings |= res.backlight > kMaxBacklight;
    badSettings |= res.autoFinishViewMinutes > kMaxAutoFinishViewMinutes;
    badSettings |= res.melody < 0;
    badSettings |= res.melody >= Melody::last_;

    if (badSettings) {
        res = kDefaultSettings;
        res.updateEEPROM();
    }

    return res;
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

bool Settings::operator==(const Settings& o) const {
    return lagTime == o.lagTime && beepVolume == o.beepVolume && backlight == o.backlight &&
           autoFinishViewMinutes == o.autoFinishViewMinutes && startWithSettings == o.startWithSettings &&
           melody == o.melody;
}
