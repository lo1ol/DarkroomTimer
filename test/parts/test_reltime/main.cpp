#include <Arduino.h>
#include <unity.h>

#include <common/RelTime.h>

void setUp() {}

void tearDown() {}

// constexpr tests
static_assert(RelTime(42).getId() == 42);

#define TEST_ASSERT_EQUAL_TIME(t1, t2) TEST_ASSERT_EQUAL((t1).toMillis(), (t2).toMillis());

void nonContstexprTests() {
    TEST_ASSERT_EQUAL_TIME(6_ts, RelTime(12 * 2) ^ 2_ts);
    TEST_ASSERT_EQUAL_TIME(8_ts, RelTime(12 * 2 + 2) ^ 2_ts);

    // checks on overflows
    TEST_ASSERT_EQUAL_TIME(3600_s, RelTime(12) ^ 3600_s);
    TEST_ASSERT_EQUAL_TIME(kBadTime, RelTime(12 + 1) ^ 3600_s); // RelTime is 1+1/2
    TEST_ASSERT_EQUAL_TIME(60108_ts, RelTime(12 + 9) ^ 3600_s); // RelTime is 1+5/12
}

void checkStringFormat() {
    char buf[16];
    RelTime(12 * 2 + 2).toStr(buf);
    TEST_ASSERT_EQUAL_STRING("2+1/3", buf);
    RelTime(12 * 2).toStr(buf);
    TEST_ASSERT_EQUAL_STRING("2", buf);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(nonContstexprTests);
    RUN_TEST(checkStringFormat);
    UNITY_END();
}
