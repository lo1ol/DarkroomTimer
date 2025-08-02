#include <Arduino.h>
#include <unity.h>

#include <Tools.h>

void setUp() {}

void tearDown() {}

void checkEncoderInt() {
    uint8_t val = 3;
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurn(1);
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(4, val);

    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(4, val);

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    // fast turn didn't work on small ranges
    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(4, val);

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 6));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(8, val);

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(3, val);

    val = 49;
    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    val = 49;
    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(50, val);

    val = 1;
    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    val = 1;
    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 50));
    TEST_ASSERT_EQUAL(0, val);

    gEncoder.emulTurnFast(-1);
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
    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    gEncoder.clear();
    TEST_ASSERT(!gEncoder.getInt(val, 0, 3));
    TEST_ASSERT_EQUAL(3, val);
}

void checkEncoderTime() {
    Time t = 3_ts;
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(300, t.toMillis());

    gEncoder.emulTurn(1);
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(300, t.toMillis());

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(400, t.toMillis());

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(300, t.toMillis());

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1100, t.toMillis());

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(300, t.toMillis());

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t, true));
    TEST_ASSERT_EQUAL(400, t.toMillis());

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t, true));
    TEST_ASSERT_EQUAL(300, t.toMillis());

    t = 1800_s - 1_ts;
    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());

    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());

    t = 1800_s - 1_ts;
    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());

    t = 1_ts;
    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(0, t.toMillis());

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(0, t.toMillis());

    t = 1_ts;
    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(0, t.toMillis());

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(0, t.toMillis());

    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1000, t.toMillis());

    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1000, t.toMillis());

    t = 5_ts;
    gEncoder.emulRetTime(1_s);
    gEncoder.tick();
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(500, t.toMillis());

    t = 1800_s + 1_ts;
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    gEncoder.clear();
    TEST_ASSERT(!gEncoder.getTime(t));
    TEST_ASSERT_EQUAL(1800000, t.toMillis());
}

void checkEncoderRelTime() {
    RelTime t{ 3 };

    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.emulTurn(1);
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(4, t.getId());

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(8, t.getId());

    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(3, t.getId());

    t = RelTime{ kMaxRelTime.getId() - 1 };
    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    gEncoder.emulTurn(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    t = RelTime{ kMaxRelTime.getId() - 1 };
    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    gEncoder.emulTurnFast(1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(kMaxRelTime.getId(), t.getId());

    t = RelTime{ 1 };
    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    gEncoder.emulTurn(-1);
    gEncoder.tick();
    TEST_ASSERT(!gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    t = RelTime{ 1 };
    gEncoder.emulTurnFast(-1);
    gEncoder.tick();
    TEST_ASSERT(gEncoder.getRelTime(t));
    TEST_ASSERT_EQUAL(0, t.getId());

    gEncoder.emulTurnFast(-1);
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
    RUN_TEST(checkEncoderInt);
    RUN_TEST(checkEncoderTime);
    RUN_TEST(checkEncoderRelTime);
    UNITY_END();
}
