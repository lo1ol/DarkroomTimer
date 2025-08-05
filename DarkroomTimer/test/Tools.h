#pragma once

#include <unity.h>

#include <Hardware.h>

#define TEST_EQUAL_SCROLL_CONTENT(scDescArg, sc)                                                                  \
    {                                                                                                             \
        ScrollableContent::Desc scDesc1 scDescArg;                                                                \
        auto&& scDesc2 = sc.getDesc();                                                                            \
        for (uint8_t i = 0; i != ScrollableContent::kMaxLineCnt; ++i)                                             \
            TEST_ASSERT_EQUAL_STRING_MESSAGE(scDesc1.lines[i], scDesc2.lines[i], "Lines mismatch");               \
                                                                                                                  \
        TEST_ASSERT_EQUAL_MESSAGE(scDesc1.currentLine, scDesc2.currentLine, "Current lines mismatch");            \
        if (scDesc1.currentLine != -1) {                                                                          \
            TEST_ASSERT_EQUAL_MESSAGE(scDesc1.currentShift, scDesc2.currentShift, "Current shifts mismatch");     \
            TEST_ASSERT_EQUAL_MESSAGE(scDesc1.currentAlign, scDesc2.currentAlign, "Current aligns mismatch");     \
            TEST_ASSERT_EQUAL_STRING_MESSAGE(scDesc1.currentMark, scDesc2.currentMark, "Current marks mismatch"); \
        }                                                                                                         \
    }

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

#define TEST_DISPLAY(line1, line2)                    \
    TEST_STRING_WITH_TRIM(line1, gLcdWrap.getLine(0)) \
    TEST_STRING_WITH_TRIM(line2, gLcdWrap.getLine(1))
