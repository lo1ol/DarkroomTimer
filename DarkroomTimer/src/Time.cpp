#include "Time.h"

#include "Config.h"
#include "DisplayLine.h"
#include "Tools.h"

void Time::getFormatedTime(char* buf, bool accurate, bool addZero) const {
    int16_t sec = secs();
    uint8_t dec = tenth();

    if (!accurate && sec > 19) {
        itoa(sec + (dec > 4), buf, 10);
        return;
    }

    if (dec == 0 && !addZero) {
        itoa(sec, buf, 10);
        return;
    }

    itoa(sec, buf, 10);
    strcpy(buf + strlen(buf), ".");
    itoa(dec, buf + strlen(buf), 10);
}

DisplayLine& operator<<(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    time.getFormatedTime(str);
    return dl << str;
}

DisplayLine& operator>>(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    time.getFormatedTime(str, true, true);
    return dl >> str;
}
