#include "Settings.h"

#ifndef DT_NATIVE
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
    GET_SETTING(res.startWith);
    GET_SETTING(res.melody);
    GET_SETTING(res.idleAfterMinutes);
    GET_SETTING(res.idleAnimation);

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
    badSettings |= res.startWith >= StartWith::last_;
    badSettings |= res.melody >= Melody::last_;
    badSettings |= res.idleAfterMinutes > kMaxIdleAfterMinutes;
    badSettings |= res.idleAnimation >= DisplayAnimation::last_;

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
    PUT_SETTING(startWith);
    PUT_SETTING(melody);
    PUT_SETTING(idleAfterMinutes);
    PUT_SETTING(idleAnimation);
    PUT_SETTING(crc32.finalize());
    #undef PUT_SETTING
}

#else
Settings Settings::load() {
    return kDefaultSettings;
}
void Settings::updateEEPROM() {}
#endif

bool Settings::operator==(const Settings& o) const {
#define RETURN_FALSE_IF_DIFFER(member) \
    do {                               \
        if (member != o.member)        \
            return false;              \
    } while (false)

    RETURN_FALSE_IF_DIFFER(lagTime);
    RETURN_FALSE_IF_DIFFER(beepVolume);
    RETURN_FALSE_IF_DIFFER(backlight);
    RETURN_FALSE_IF_DIFFER(autoFinishViewMinutes);
    RETURN_FALSE_IF_DIFFER(startWith);
    RETURN_FALSE_IF_DIFFER(melody);
    RETURN_FALSE_IF_DIFFER(idleAfterMinutes);
    RETURN_FALSE_IF_DIFFER(idleAnimation);

#undef RETURN_FALSE_IF_DIFFER

    return true;
}
