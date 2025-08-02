#pragma once

#include "../VirtEnv.h"

#include <stdint.h>
#include <string.h>

#define TEST_STRING_WITH_TRIM(line, nonTrimmed)         \
    {                                                   \
        char lineBuf[DISPLAY_COLS + 1];                 \
        strcpy(lineBuf, (nonTrimmed));                  \
                                                        \
        for (int8_t i = strlen(lineBuf); i != 0; --i) { \
            if (lineBuf[i - 1] != ' ')                  \
                break;                                  \
            lineBuf[i - 1] = 0;                         \
        }                                               \
        TEST_ASSERT_EQUAL_STRING((line), lineBuf);      \
    }

#define TEST_DISPLAY(line1, line2)                        \
    TEST_STRING_WITH_TRIM(line1, gLcdWrapMock.getLine(0)) \
    TEST_STRING_WITH_TRIM(line2, gLcdWrapMock.getLine(1))
