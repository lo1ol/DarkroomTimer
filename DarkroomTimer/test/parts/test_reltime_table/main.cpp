#include <Arduino.h>
#include <unity.h>

#include <Hardware.h>
#include <RelTimeTable.h>

#include "../../Tools.h"

void setUp() {
    gScrollableContent.reset();
}

void tearDown() {}

void checkRelTimeTableGeneric() {
    RelTimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    tt.setBaseTime(4_ts);

    for (int i = 0; i != tt.capacity(); ++i)
        tt.setRelTime(i, RelTime(i));

    for (int i = 0; i != tt.capacity(); ++i) {
        TEST_ASSERT(tt.getRelTime(i) == RelTime(i));
        TEST_ASSERT(tt.getTime(i) == (RelTime(i) ^ 4_ts));
    }

    tt.setRelTime(1, RelTime(2));
    TEST_ASSERT(tt.getRelTime(1) == RelTime(2));
    TEST_ASSERT(tt.getTime(1) == (RelTime(2) ^ 4_ts));
    TEST_ASSERT(tt.getTime(-1) == 4_ts);
    TEST_ASSERT(tt.getBaseTime() == 4_ts);

    tt.setBaseTime(8_ts);
    TEST_ASSERT(tt.getTime(-1) == 8_ts);
    TEST_ASSERT(tt.getBaseTime() == 8_ts);

    tt.setRelTime(1, RelTime(1));

    for (int i = 0; i != tt.capacity(); ++i) {
        TEST_ASSERT(tt.getRelTime(i) == RelTime(i));
        TEST_ASSERT(tt.getTime(i) == (RelTime(i) ^ 8_ts));
    }
}

void checkRelTimeTablePrint() {
    RelTimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    tt.setBaseTime(4_s);

    for (int i = 0; i != 6; ++i)
        tt.setRelTime(i, RelTime(i));

    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1/2 1/3 2/3", "1/4 3/4" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1/2 1/3 2/3", "1/4 3/4", "4 0 1/2 1/3 2/3", "1/4 3/4" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setRelTime(3, RelTime(11));
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1/2 1/3", "11/12 1/4 3/4" },
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
                                  .lines = { "4 0 1/2 1/3", "11/12 1/4 3/4" },
                                  .currentLine = 1,
                                  .currentShift = 0,
                                  .currentAlign = 5,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(2);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1/2 1/3", "11/12 1/4 3/4" },
                                  .currentLine = 0,
                                  .currentShift = 8,
                                  .currentAlign = 3,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.resize(2);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1/2" },
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
                                  .lines = { "kek 4 0 1/2" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setPrefix("lolo");
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "kek 4 0 1/2", "lolo 4 0 1/2" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setBaseTime(1000_s);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000 0 1/2" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(-1);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000.0 0", "1/2" },
                                  .currentLine = 0,
                                  .currentShift = 5,
                                  .currentAlign = 6,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(1);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000 0", "  1/2" },
                                  .currentLine = 1,
                                  .currentShift = 0,
                                  .currentAlign = 5,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);
}

void checkRelTimeTableOverflow() {
    RelTimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    tt.setBaseTime(2000_s);

    RelTime times[] = { RelTime(12), RelTime(13), RelTime(24), RelTime(14), RelTime(21) };
    int i = 0;
    for (auto time : times) {
        tt.setRelTime(i, time);
        ++i;
    }

    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "2000 1 ovr ovr", "1+1/3 1+5/12" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setCurrent(1);
    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "2000 1 1+1/2 ovr", "1+1/3 1+5/12" },
                                  .currentLine = 0,
                                  .currentShift = 7,
                                  .currentAlign = 5,
                                  .currentMark = "ovr",
                              }),
                              gScrollableContent);

    tt.setCurrent(-2);
    tt.setSecView(true);
    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "2000 2000 ovr", "ovr 3040 3339" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setCurrent(1);
    tt.setSecView(true);
    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "2000 2000 0.0", "ovr 3040 3339" },
                                  .currentLine = 0,
                                  .currentShift = 10,
                                  .currentAlign = 3,
                                  .currentMark = "ovr",
                              }),
                              gScrollableContent);

    tt.setCurrent(-2);
    tt.setBaseTime(1300_s);
    tt.setSecView(false);
    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "1300 1 1+1/2 ovr", "1+1/3 1+5/12" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setBaseTime(1000_s);
    gScrollableContent.reset();
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "1000 1 1+1/2 2", "1+1/3 1+5/12" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);
}

void checkRelTimeTablePrintSecView() {
    RelTimeTable tt;
    tt.setBuffer(gModesCache, sizeof(gModesCache));
    tt.setBaseTime(4_s);

    for (int i = 0; i != 6; ++i)
        tt.setRelTime(i, RelTime(i));

    tt.setSecView(true);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1.7 1 2.3", "0.8 2.7" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1.7 1 2.3", "0.8 2.7", "4 0 1.7 1 2.3", "0.8 2.7" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setRelTime(3, RelTime(11));
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1.7 1 3.6", "0.8 2.7" },
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
                                  .lines = { "4 0 1.7 1 3.6", "0.8 2.7" },
                                  .currentLine = 0,
                                  .currentShift = 10,
                                  .currentAlign = 3,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(2);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1.7 1.0 3.6", "0.8 2.7" },
                                  .currentLine = 0,
                                  .currentShift = 8,
                                  .currentAlign = 3,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.resize(2);
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "4 0 1.7" },
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
                                  .lines = { "kek 4 0 1.7" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    tt.setPrefix("lolo");
    tt.flush();

    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "kek 4 0 1.7", "lolo 4 0 1.7" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setBaseTime(1000_s);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000 0 414" },
                                  .currentLine = -1,
                                  .currentShift = 0,
                                  .currentAlign = 0,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(-1);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000.0 0", "414" },
                                  .currentLine = 0,
                                  .currentShift = 5,
                                  .currentAlign = 6,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);

    gScrollableContent.reset();
    tt.setCurrent(1);
    tt.flush();
    TEST_EQUAL_SCROLL_CONTENT(({
                                  .lines = { "lolo 1000 0", "414.2" },
                                  .currentLine = 1,
                                  .currentShift = 0,
                                  .currentAlign = 5,
                                  .currentMark = nullptr,
                              }),
                              gScrollableContent);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkRelTimeTableGeneric);
    RUN_TEST(checkRelTimeTablePrint);
    RUN_TEST(checkRelTimeTablePrintSecView);
    RUN_TEST(checkRelTimeTableOverflow);
    UNITY_END();
}
