#include "Utils.h"

#include "Config.h"

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
