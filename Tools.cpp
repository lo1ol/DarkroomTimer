#include "Tools.h"

#include <string.h>

LiquidCrystal gLcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
EncButton gEncoder(ENCODER_DT, ENCODER_CLK);
Button gModeSwitchBtn(MODE_SWITCH_BTN);
Button gStartBtn(START_BTN);
Button gExtraBtn(EXTRA_BTN);
Button gViewBtn(VIEW_BTN);
Timer gTimer(BEEPER, RELAY);
Settings gSettings;

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

void getFormatedTime(uint32_t ms, char* buf, bool accurate) {
    long decs = lround(ms / float(100));
    ;
    uint16_t sec = decs / 10;
    uint8_t dec = decs % 10;

    if (!accurate && sec > 9) {
        itoa(sec + (dec > 4), buf, 10);
        return;
    }

    if (dec == 0) {
        itoa(sec, buf, 10);
        return;
    }

    itoa(sec, buf, 10);
    concat(buf, ".");
    concatInt(buf, dec);
}

void printFormatedTime(const char* prefix, uint32_t ms) {
    char str[MAX_SYMS_PER_LINE + 1];
    strcpy(str, prefix);
    concatTime(str, ms);

    printFormatedLine(str, 1);
}

uint8_t printTimeLogOnLine(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex, uint8_t index,
                           int pos) {
    char str[MAX_SYMS_PER_LINE + 1];
    char buf[4];

    strcpy(str, prefix);
    uint8_t shift = strlen(prefix);

    while (true) {
        if (index == maxIndex)
            break;

        getFormatedTime(timeGetter(index), buf, false);
        uint8_t bufLen = strlen(buf);
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

void printTimeLog(const char* prefix, uint32_t (*timeGetter)(uint8_t), uint8_t maxIndex) {
    uint8_t lastNonPrintedIndex = printTimeLogOnLine(prefix, timeGetter, maxIndex, 0, 0);
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

int8_t getEncoderShift() {
    if (!gEncoder.turn())
        return 0;
    return gEncoder.dir() * (gEncoder.fast() ? 5 : 1);
}

void getInt(uint8_t& choosen, uint8_t min, uint8_t max) {
    int8_t shift = getEncoderShift();

    if (shift < 0 && (choosen - min) < -shift)
        choosen = min;
    else if (shift > 0 && (max - choosen) < shift)
        choosen = max;
    else
        choosen += shift;
}

void getTime(uint32_t& time) {
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
