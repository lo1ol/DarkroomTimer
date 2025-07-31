#include <Arduino.h>
#include "unity.h"

#include "../VirtEnv.h"

void setUp() {
    initVirtEnv();
}

void tearDown() {}

void checkTimerStart() {
    gTimer.start(1_s);
    TEST_ASSERT_EQUAL(false, gTimer.stopped());
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());

    gCurrentTime += 999;
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    gCurrentTime += 1;
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(true, gTimer.stopped());

    gTimer.start(0_s);
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(true, gTimer.stopped());
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(false, gTimer.stopped());
}

void checkTimerPauseResume() {
    gTimer.start(2_s);
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());

    gCurrentTime += 950;
    gTimer.tick();
    TEST_ASSERT_EQUAL(1050, gTimer.left());
    TEST_ASSERT_EQUAL(900, gTimer.afterLastResume().toMillis());
    gTimer.pause();
    TEST_ASSERT_EQUAL(Timer::State::PAUSED, gTimer.state());
    TEST_ASSERT_EQUAL(1050, gTimer.left());
    TEST_ASSERT_EQUAL(900, gTimer.afterLastResume().toMillis());
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::PAUSED, gTimer.state());
    TEST_ASSERT_EQUAL(1050, gTimer.left());
    TEST_ASSERT_EQUAL(900, gTimer.afterLastResume().toMillis());

    gCurrentTime += 3000;
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::PAUSED, gTimer.state());
    TEST_ASSERT_EQUAL(1050, gTimer.left());
    TEST_ASSERT_EQUAL(900, gTimer.afterLastResume().toMillis());

    gTimer.resume();
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    TEST_ASSERT_EQUAL(1050, gTimer.left());
    // timer count time after resume only after it became greater then 0
    TEST_ASSERT_EQUAL(900, gTimer.afterLastResume().toMillis());

    gCurrentTime += 799;
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    TEST_ASSERT_EQUAL(251, gTimer.left());
    TEST_ASSERT_EQUAL(800, gTimer.afterLastResume().toMillis());

    gCurrentTime += 251;
    gTimer.tick();
    gTimer.pause();
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(true, gTimer.stopped());
    // we are not paused
    TEST_ASSERT_EQUAL(1000, gTimer.afterLastResume().toMillis());
}

void checkTimerStop() {
    gTimer.start(2_s);
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    gTimer.tick();
    gTimer.stop();
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(false, gTimer.stopped());
    gTimer.tick();
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
    TEST_ASSERT_EQUAL(false, gTimer.stopped());
}

void checkTimerTotal() {
    gTimer.start(2_s);
    gTimer.tick();
    gCurrentTime += 2500;
    gTimer.tick();
    TEST_ASSERT_EQUAL(2000, gTimer.total().toMillis());
    TEST_ASSERT_EQUAL(2000, gTimer.afterLastResume().toMillis());
    gTimer.reset();

    gTimer.start(2_s);
    gTimer.tick();
    gCurrentTime += 1000;
    gTimer.tick();
    TEST_ASSERT_EQUAL(1000, gTimer.total().toMillis());
    gTimer.reset();

    gTimer.start(2_s);
    gTimer.tick();
    gCurrentTime += 3000;
    gTimer.tick();

    gTimer.start(3_s);
    gTimer.tick();
    gCurrentTime += 2000;
    gTimer.tick();
    TEST_ASSERT_EQUAL(4000, gTimer.total().toMillis());
    gTimer.resetTotal();
    // timer still is on and 2 seconds passed
    TEST_ASSERT_EQUAL(2000, gTimer.total().toMillis());

    gCurrentTime += 1000;
    gTimer.tick();
    TEST_ASSERT_EQUAL(3000, gTimer.total().toMillis());
    gTimer.resetTotal();
    TEST_ASSERT_EQUAL(0, gTimer.total().toMillis());
}

void checkTimerLag() {
    gSettings.lagTime = 1_s;
    gTimer.start(2_s);
    gTimer.tick();
    TEST_ASSERT_EQUAL(true, gTimer.lag());
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());

    gCurrentTime += 500;
    gTimer.tick();
    TEST_ASSERT_EQUAL(0, gTimer.afterLastResume().toMillis());
    TEST_ASSERT_EQUAL(true, gTimer.lag());
    TEST_ASSERT_EQUAL(2000, gTimer.left());
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());

    gCurrentTime += 501;
    gTimer.tick();
    TEST_ASSERT_EQUAL(0, gTimer.afterLastResume().toMillis());
    TEST_ASSERT_EQUAL(false, gTimer.lag());
    TEST_ASSERT_EQUAL(1999, gTimer.left());
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());

    gCurrentTime += 1899;
    gTimer.tick();
    TEST_ASSERT_EQUAL(1900, gTimer.afterLastResume().toMillis());
    TEST_ASSERT_EQUAL(false, gTimer.lag());
    TEST_ASSERT_EQUAL(100, gTimer.left());
    TEST_ASSERT_EQUAL(Timer::State::RUNNING, gTimer.state());
    TEST_ASSERT_EQUAL(1900, gTimer.total().toMillis());

    gCurrentTime += 100;
    gTimer.tick();
    TEST_ASSERT_EQUAL(2000, gTimer.afterLastResume().toMillis());
    TEST_ASSERT_EQUAL(false, gTimer.lag());
    TEST_ASSERT_EQUAL(0, gTimer.left());
    TEST_ASSERT_EQUAL(2000, gTimer.total().toMillis());
    TEST_ASSERT_EQUAL(Timer::State::STOPPED, gTimer.state());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkTimerStart);
    RUN_TEST(checkTimerPauseResume);
    RUN_TEST(checkTimerStop);
    RUN_TEST(checkTimerTotal);
    RUN_TEST(checkTimerLag);
    UNITY_END();
}
