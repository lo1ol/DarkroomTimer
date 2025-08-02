#pragma once

#include <ScrollableContent.h>
#include <unity.h>

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
