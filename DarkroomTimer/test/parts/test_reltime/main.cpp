#include <Arduino.h>
#include <unity.h>

#include <RelTime.h>

void setUp() {}

void tearDown() {}

// constexpr tests
static_assert(RelTime(42).getId() == 42);

void nonContstexprTests() {
    TEST_ASSERT_EQUAL(Time(6).toMillis(), (RelTime(12 * 2) ^ 2_ts).toMillis());
    TEST_ASSERT_EQUAL(Time(8).toMillis(), (RelTime(12 * 2 + 2) ^ 2_ts).toMillis());

    // checks on overflows
    TEST_ASSERT_EQUAL(Time(18000).toMillis(), (RelTime(12) ^ 1800_s).toMillis());
    TEST_ASSERT_EQUAL(kBadTime.toMillis(), (RelTime(12 + 1) ^ 1800_s).toMillis());    // RelTime is 2+1/2
    TEST_ASSERT_EQUAL(Time(30054).toMillis(), (RelTime(12 + 9) ^ 1800_s).toMillis()); // RelTime is 2+5/12
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
