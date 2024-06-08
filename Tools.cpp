#include "Tools.h"

#include <string.h>

LiquidCrystal gLcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
EncButton gEncoder(ENCODER_DT, ENCODER_CLK);
Button gStartBtn(START_BTN);
Button gExtraBtn(EXTRA_BTN);
Button gViewBtn(VIEW_BTN);
Timer gTimer(BEEPER, RELAY);

void printFormatedLine(const char* line, int pos) {
    gLcd.setCursor(0, pos);
    char formatedLine[MAX_SYMS_PER_LINE + 1];
    strcpy(formatedLine, line);
    unsigned len = strlen(line);
    if (len > MAX_SYMS_PER_LINE)
        len = MAX_SYMS_PER_LINE;

    memcpy(formatedLine, line, len);
    memset(formatedLine + len, ' ', MAX_SYMS_PER_LINE - len);

    formatedLine[MAX_SYMS_PER_LINE] = 0;
    gLcd.print(formatedLine);
}

void getFormatedTime(unsigned long ms, char* buf, bool accurate) {
    long decs = lround(ms / 100.);
    ;
    unsigned long sec = decs / 10;
    unsigned long dec = decs % 10;

    if (!accurate && sec > 9) {
        sprintf(buf, "%lu", sec + (dec > 4));
        return;
    }

    if (dec == 0)
        sprintf(buf, "%lu", sec);
    else
        sprintf(buf, "%lu.%lu", sec, dec);
}

void printFormatedTime(const char* prefix, unsigned long ms) {
    char str[MAX_SYMS_PER_LINE + 1];
    strcpy(str, prefix);
    getFormatedTime(ms, str + strlen(prefix));

    printFormatedLine(str, 1);
}

size_t printTimeLogOnLine(const char* prefix, unsigned long (*timeGetter)(size_t), size_t maxIndex, size_t index,
                          int pos) {
    char str[MAX_SYMS_PER_LINE + 1];
    char buf[4];

    strcpy(str, prefix);
    size_t shift = strlen(prefix);

    while (true) {
        if (index == maxIndex)
            break;

        getFormatedTime(timeGetter(index), buf, false);
        size_t bufLen = strlen(buf);
        if ((bufLen + shift) >= sizeof(str))
            break;
        strcpy(str + shift, buf);
        shift += bufLen;
        ++index;

        if (shift + 1 >= sizeof(str))
            break;

        str[shift] = ' ';
        str[shift + 1] = 0;
        ++shift;
    }

    printFormatedLine(str, pos);

    return index;
}

void printTimeLog(const char* prefix, unsigned long (*timeGetter)(size_t), size_t maxIndex) {
    size_t lastNonPrintedIndex = printTimeLogOnLine(prefix, timeGetter, maxIndex, 0, 0);
    if (lastNonPrintedIndex == maxIndex)
        printFormatedLine("", 1);
    else
        printTimeLogOnLine("", timeGetter, maxIndex, lastNonPrintedIndex, 1);
}

void isr() {
    gEncoder.tickISR();
}

void setupEncoder() {
    attachInterrupt(0, isr, CHANGE);
    attachInterrupt(1, isr, CHANGE);
    gEncoder.setEncISR(true);
}

int getEncoderShift() {
    if (!gEncoder.turn())
        return 0;
    return gEncoder.dir() * (gEncoder.fast() ? 5 : 1);
}

void getInt(size_t& choosen, size_t min, size_t max) {
    int shift = getEncoderShift();

    if (choosen == min && shift == -1)
        return;

    if (choosen == max && shift == 1)
        return;

    choosen += shift;
}

void getTime(unsigned long& time) {
    int shift = getEncoderShift();

    int factor;
    if ((time + shift) < 10000)
        factor = 500;
    else if ((time + shift) < 100000)
        factor = 1000;
    else
        factor = 5000;

    shift *= factor;

    if (shift < 0 && time < -shift) {
        time = 0;
    } else {
        time += shift;
        time -= time % factor;
    }
}
