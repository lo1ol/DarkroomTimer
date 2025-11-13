#include "Utils.h"

#include "Config.h"

#include <math.h>
#include <string.h>

void alignStr(char* buf, uint8_t align) {
    uint8_t len = strlen(buf);
    if (align <= len) {
        buf[align] = 0;
        return;
    }

    char alignedStr[DISPLAY_COLS + 1];
    memset(alignedStr, ' ', align);
    strcpy(alignedStr + align - len, buf);
    strcpy(buf, alignedStr);
}

float fast2pow(float exp) {
    assert(exp >= 0);

    int8_t intPart = static_cast<int8_t>(exp);
    float fracPart = exp - intPart;

    // First 5 terms of Taylor series 2**(log2(e)*x)
    float mantisa =
        1.0f + fracPart * (0.69314718f +
                           fracPart * (0.24022651f +
                                       fracPart * (0.05550411f + fracPart * (0.00961813f + fracPart * 0.00133f))));

    return ldexpf(mantisa, intPart);
}
