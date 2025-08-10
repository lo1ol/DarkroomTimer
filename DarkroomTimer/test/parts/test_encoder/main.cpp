#include <Arduino.h>
#include <unity.h>

#include <Hardware.h>

void setUp() {
    gEncoder.clear();
}

void tearDown() {}

#define TEST_ASSERT_EQUAL_TIME(t1, t2) TEST_ASSERT_EQUAL((t1).toMillis(), (t2).toMillis());

void checkEncoderDir() {
    gEncoder.emulTurns(2);
    TEST_ASSERT_EQUAL(0, gEncoder.getDir());

    gEncoder.tick();
    TEST_ASSERT_EQUAL(1, gEncoder.getDir());

    gEncoder.emulTurns(-3);
    gEncoder.tick();
    TEST_ASSERT_EQUAL(-1, gEncoder.getDir());

    gEncoder.emulFastTurns(-3);
    gEncoder.tick();
    TEST_ASSERT_EQUAL(-1, gEncoder.getDir());

    gEncoder.emulTurns(3);
    gEncoder.emulFastTurns(-3);
    gEncoder.tick();
    TEST_ASSERT_EQUAL(0, gEncoder.getDir());

    gEncoder.emulTurns(4);
    gEncoder.emulFastTurns(-3);
    gEncoder.tick();
    TEST_ASSERT_EQUAL(1, gEncoder.getDir());

    gEncoder.emulTurns(-7);
    gEncoder.emulFastTurns(3);
    gEncoder.emulFastFastTurns(2);
    gEncoder.tick();
    TEST_ASSERT_EQUAL(-1, gEncoder.getDir());
}

void checkEncoderInt() {
    uint8_t val = 3;
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurns(2);
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(5, val);

    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(5, val);

    gEncoder.emulTurns(-2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    // fast turn didn't work on small ranges
    gEncoder.emulFastTurns(2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(5, val);

    gEncoder.emulFastTurns(-2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulFastTurns(2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(7, val);

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(5, val);

    val = 49;
    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    val = 49;
    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    val = 1;
    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    val = 1;
    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    gEncoder.emulRetInt(7);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(7, val);

    gEncoder.emulRetInt(7);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(7, val);

    val = 2;
    gEncoder.emulRetInt(7);
    gEncoder.tick();
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(2, val);

    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(2, val);

    val = 5;
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    val = 5;
    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    gEncoder.clear();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    val = 5;
    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    gEncoder.clear();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurns(2);
    gEncoder.emulFastTurns(3);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 100));
    TEST_ASSERT_EQUAL(11, val);

    gEncoder.emulTurns(2);
    gEncoder.emulFastTurns(3);
    gEncoder.emulFastFastTurns(-3);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 100));
    TEST_ASSERT_EQUAL(7, val);
}

void checkEncoderTime() {
    Time t = 3_ts;
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(3_ts, t);

    gEncoder.emulTurns(1);
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(3_ts, t);

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(4_ts, t);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(3_ts, t);

    gEncoder.emulFastTurns(2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(9_ts, t);

    gEncoder.emulFastTurns(-2);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(3_ts, t);

    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t, true));
    TEST_ASSERT_EQUAL_TIME(4_ts, t);

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t, true));
    TEST_ASSERT_EQUAL_TIME(3_ts, t);

    t = kMaxTime - 1_ts;
    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    t = kMaxTime - 1_ts;
    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    t = 1_ts;
    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(0_ts, t);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(0_ts, t);

    t = 1_ts;
    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(0_ts, t);

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(0_ts, t);

    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(10_ts, t);

    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(10_ts, t);

    t = 5_ts;
    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(5_ts, t);

    t = kMaxTime;
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    gEncoder.clear();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(36000_ts, t);

    t = kBadTime;
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(0_ts, t);

    t = 0_s;
    gEncoder.emulTurns(2);
    gEncoder.emulFastTurns(-1);
    gEncoder.emulFastFastTurns(3);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL_TIME(17_ts, t);
}

void checkEncoderRelTime() {
    RelTime t{ 3 };

    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.emulTurns(1);
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(4, t.getId());

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(5, t.getId());

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    t = RelTime{ kMaxRelTime.getId() - 1 };
    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    gEncoder.emulTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    t = RelTime{ kMaxRelTime.getId() - 1 };
    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    gEncoder.emulFastTurns(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    t = RelTime{ 1 };
    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    gEncoder.emulTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    t = RelTime{ 1 };
    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    gEncoder.emulFastTurns(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    gEncoder.emulRetRelTime(RelTime{ 7 });
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(7, t.getId());

    gEncoder.emulRetRelTime(RelTime{ 7 });
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(7, t.getId());

    t = RelTime{ 2 };
    gEncoder.emulRetRelTime(RelTime{ 7 });
    gEncoder.tick();
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(2, t.getId());

    t = RelTime{ kMaxRelTime.getId() + 1 };
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkEncoderDir);
    RUN_TEST(checkEncoderInt);
    RUN_TEST(checkEncoderTime);
    RUN_TEST(checkEncoderRelTime);
    UNITY_END();
}
