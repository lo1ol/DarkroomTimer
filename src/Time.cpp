#include "Time.h"

#include "Config.h"
#include "DisplayLine.h"
#include "Tools.h"

namespace {
void getFormatedTime(const Time& time, char* buf, bool addZero = false) {
    int16_t sec = time.secs();
    uint8_t dec = time.tenth();

    if (sec > 9) {
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

uint8_t printTimeLogOnLine(const char* prefix, Time (*timeGetter)(uint8_t), uint8_t maxIndex, uint8_t index,
                           DisplayLine& dl) {
    dl << prefix;

    while (true) {
        if (index == maxIndex)
            break;

        char str[DISPLAY_COLS + 1] = { 0 };
        getFormatedTime(timeGetter(index), str);
        if (!dl.tryPrint(str))
            return index;
        dl << " ";
        ++index;
    }

    return index;
}

} // namespace

void Time::printTimeLog(const char* prefix, Time (*timeGetter)(uint8_t), uint8_t maxIndex) {
    uint8_t lastNonPrintedIndex = 0;
    for (uint8_t id = 0; id != DISPLAY_ROWS; ++id) {
        lastNonPrintedIndex =
            printTimeLogOnLine(prefix, timeGetter, maxIndex, lastNonPrintedIndex, gDisplay.getLines()[id]);
        prefix = "";
    }
}

DisplayLine& operator<<(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    getFormatedTime(time, str);
    return dl << str;
}

DisplayLine& operator>>(DisplayLine& dl, const Time& time) {
    char str[DISPLAY_COLS + 1];
    getFormatedTime(time, str, true);
    return dl >> str;
}
