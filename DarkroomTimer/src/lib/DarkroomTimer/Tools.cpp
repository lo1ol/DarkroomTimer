#include "Tools.h"

EncButton gEncoder(ENCODER_DT, ENCODER_CLK);
ButtonT<MODE_SWITCH_BTN> gModeSwitchBtn;
ButtonT<START_BTN> gStartBtn;
ButtonT<EXTRA_BTN> gExtraBtn;
ButtonT<VIEW_BTN> gViewBtn;
Timer gTimer;
Settings gSettings;
Display gDisplay(LiquidCrystal(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7));
Beeper gBeeper(BEEPER);
uint8_t gModesCache[32];
ScrollableContent gScrollableContent;

void isr() {
    gEncoder.tickISR();
}

void setupEncoder() {
    attachInterrupt(0, isr, CHANGE);
    attachInterrupt(1, isr, CHANGE);
    gEncoder.setEncISR(true);
}

int8_t getEncoderDir() {
    if (!gEncoder.turn())
        return 0;
    return gEncoder.dir();
}

bool getInt(uint8_t& choosen, uint8_t min, uint8_t max) {
    int8_t shift = getEncoderDir();
    if (max - min > 30 && gEncoder.fast())
        shift *= 5;

    if (shift == 0)
        return false;

    if (shift < 0 && (choosen - min) < -shift) {
        choosen = min;
        return false;
    } else if (shift > 0 && (max - choosen) < shift) {
        choosen = max;
        return false;
    } else {
        choosen += shift;
        return true;
    }
}

bool getTime(Time& time, bool smooth) {
    Time shift{ getEncoderDir() };
    if (!smooth && gEncoder.fast())
        shift *= 8;

    Time oldTime = time;

    // Values are changing with accuracy 1/12 stop
    // You can check accuracy range by this formule:
    // t*(2**(1/12)-1)
    int16_t factor;
    if ((time + shift) < 10_s)
        factor = 1;
    else if ((time + shift) < 20_s)
        factor = 5;
    else if ((time + shift) < 100_s)
        factor = 10;
    else if ((time + shift) < 200_s)
        factor = 50;
    else if ((time + shift) < 1000_s)
        factor = 100;
    else
        factor = 500;

    time += shift * factor;
    time = Time((static_cast<int16_t>(time) / factor) * factor);

    if (time < 0_ts)
        time = 0_ts;

    if (time > 1800_s)
        time = 1800_s;

    return time != oldTime;
}

bool getRelTime(RelTime& time) {
    uint8_t relTimeId = time.getId();
    bool res = getInt(relTimeId, 0, kMaxRelTime.getId());

    time = RelTime(relTimeId);
    return res;
}

void alignStr(char* buf, uint8_t align) {
    char alignedStr[DISPLAY_COLS + 1];
    memset(alignedStr, ' ', align);
    uint8_t len = strlen(buf);
    strcpy(alignedStr + align - len, buf);
    strcpy(buf, alignedStr);
}
