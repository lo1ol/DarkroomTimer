#include "Settings.h"

#include <CRC32.h>
#include <EEPROM.h>

Settings Settings::load() {
    Settings res;
    int idx = 0;
    CRC32 crc32;
#define GET_SETTING(value)    \
    EEPROM.get(idx, (value)); \
    idx += sizeof(value);     \
    crc32.update(value);

    GET_SETTING(res.bwBaseTime);
    GET_SETTING(res.colorBaseTime);

    uint32_t hash = crc32.finalize();
    uint32_t storedHash;
    GET_SETTING(storedHash);
#undef GET_SETTING

    bool badSettings = false;
    badSettings |= hash != storedHash;
    badSettings |= res.bwBaseTime.baseLogD > 999;
    badSettings |= res.bwBaseTime.baseTime > kMaxTime;
    badSettings |= res.colorBaseTime.baseLogD > 999;
    badSettings |= res.colorBaseTime.baseTime > kMaxTime;

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

    PUT_SETTING(bwBaseTime);
    PUT_SETTING(colorBaseTime);
    PUT_SETTING(crc32.finalize());
#undef PUT_SETTING
}
