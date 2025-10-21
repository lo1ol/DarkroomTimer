#include "RelTime.h"

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

namespace {

constexpr uint8_t kFracIdMap[12] = { 0, 6, 4, 8, 3, 9, 2, 10, 1, 5, 7, 11 };
constexpr const char* kFracNameMap[12] = {
    nullptr, "1/2", "1/3", "2/3", "1/4", "3/4", "1/6", "5/6", "1/12", "5/12", "7/12", "11/12",
};

} // namespace

uint8_t RelTime::toStr(char* buf) const {
    if (m_id == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return 1;
    }

    uint8_t whole = m_id / 12;
    uint8_t fracId = m_id % 12;

    uint8_t shift = 0;
    if (whole) {
        itoa(whole, buf, 10);
        shift = strlen(buf);
    }

    const char* fracStr = kFracNameMap[fracId];

    if (!fracStr)
        return shift;

    if (whole)
        buf[shift++] = '+';
    strcpy(buf + shift, fracStr);

    return shift + strlen(fracStr);
}

Time RelTime::operator^(Time t) const {
    uint8_t whole = m_id / 12;
    uint8_t frac = kFracIdMap[m_id % 12];
    float val = whole + frac / 12.;

    return t * (pow(2, val) - 1);
}
