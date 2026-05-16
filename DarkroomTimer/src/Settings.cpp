#include "Settings.h"

#ifndef DT_NATIVE
    #include <EEPROM.h>

    #include "Config.h"

namespace {
// https://en.wikipedia.org/wiki/BSD_checksum
void hash_update(uint16_t& hash, const void* data, size_t size) {
    const uint8_t* ptr = (const uint8_t*)data;
    while (size--) {
        hash = (hash >> 1) + ((hash & 1) << 15);
        hash += *ptr++;
    }
}
} // namespace

Settings Settings::load() {
    Settings res;
    int idx = 0;
    uint16_t hash = 0;

    #define GET_SETTING(value)    \
        EEPROM.get(idx, (value)); \
        idx += sizeof(value);     \
        hash_update(hash, &value, sizeof(value));

    GET_SETTING(res.lagTime);
    GET_SETTING(res.beepVolume);
    GET_SETTING(res.backlight);
    GET_SETTING(res.autoFinishViewMinutes);
    GET_SETTING(res.startWith);
    GET_SETTING(res.melody);
    GET_SETTING(res.idleAfterMinutes);
    GET_SETTING(res.idleAnimation);

    uint16_t computedHash = hash;
    uint16_t storedHash;
    GET_SETTING(storedHash);

    #undef GET_SETTING

    bool badSettings = false;
    badSettings |= storedHash != computedHash;
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
    uint16_t hash = 0;

    #define PUT_SETTING(value)    \
        EEPROM.put(idx, (value)); \
        idx += sizeof(value);     \
        hash_update(hash, &value, sizeof(value));

    PUT_SETTING(lagTime);
    PUT_SETTING(beepVolume);
    PUT_SETTING(backlight);
    PUT_SETTING(autoFinishViewMinutes);
    PUT_SETTING(startWith);
    PUT_SETTING(melody);
    PUT_SETTING(idleAfterMinutes);
    PUT_SETTING(idleAnimation);
    PUT_SETTING(hash);
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
