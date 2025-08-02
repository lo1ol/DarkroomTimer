#include <Arduino.h>
#include <unity.h>

#include <TimeTable.h>
#include <Tools.h>

#include "../../Tools.h"

void setUp() {
    gScrollableContent.reset();
}

void tearDown() {}

void checkTimeTableGeneric() {
    TimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    for (int i = 0; i != tt.capacity(); ++i)
        tt.setTime(i, Time(i));

    for (int i = 0; i != tt.capacity(); ++i)
        TEST_ASSERT(tt.getTime(i) == Time(i));

    tt.setTime(1, 2_ts);
    TEST_ASSERT(tt.getTime(1) == 2_ts);
}

void checkTimeTablePrint() {
    TimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    for (int i = 0; i != 6; ++i)
        tt.setTime(i, Time(i));

    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 0.3", "0.4 0.5" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 0.3", "0.4 0.5", "0 0.1 0.2 0.3", "0.4 0.5" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setTime(3, 3_s);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 3 0.4", "0.5" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();

    tt.setCurrent(3);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 3.0", "0.4 0.5" },
                                  .currentLine = 0,
                                  .currentShift = 10,
                                  .currentAlign = 3,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 3.0", "0.4 0.5", "0 0.1 0.2 3.0", "0.4 0.5" },
                                  .currentLine = 2,
                                  .currentShift = 10,
                                  .currentAlign = 3,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(2, "HA");
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.2 3 0.4", "0.5" },
                                  .currentLine = 0,
                                  .currentShift = 6,
                                  .currentAlign = 3,
                                  .currentMark = "HA",
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.resize(2);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setTime(2, 4_ts);
    tt.setTime(3, kBadTime);
    tt.setTime(4, 6_ts);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.4" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.printBadAsZero(true);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "0 0.1 0.4 0 0.6" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setPrefix("kek");
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "kek 0 0.1 0.4 0", "0.6" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setPrefix("lolo");
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "kek 0 0.1 0.4 0", "0.6", "lolo 0 0.1 0.4 0", "0.6" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkTimeTableGeneric);
    RUN_TEST(checkTimeTablePrint);
    UNITY_END();
}
