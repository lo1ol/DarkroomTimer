#include <Arduino.h>
#include <unity.h>

#include <RelTime.h>

void setUp() {}

void tearDown() {}

// constexpr tests
static_assert(RelTime(42).getId() == 42);

void nonContstexprTests() {
    TEST_ASSERT((RelTime(12 * 2) ^ 2_ts) == 6_ts);
    TEST_ASSERT((RelTime(12 * 2 + 2) ^ 2_ts) == 8_ts);
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
