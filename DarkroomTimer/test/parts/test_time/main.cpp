#include <Arduino.h>
#include <unity.h>

#include <Time.h>

void setUp() {}

void tearDown() {}

// constexpr tests
static_assert(Time(42).secs() == 4);
static_assert(Time(42).tenth() == 2);

static_assert(42_ts + 15_ts == 57_ts);
static_assert(42_ts - 15_ts == 27_ts);
static_assert(42_ts * 2 == 84_ts);
static_assert(42_ts / 5 == 8_ts);
static_assert(-Time(42) == Time(-42));
static_assert(Time(42) == -Time(-42));
static_assert(Time(42) != -Time(43));
static_assert(42_ts > -42_ts);
static_assert(Time(INT16_MAX) == kBadTime);
static_assert(Time(INT16_MAX - 1) != kBadTime);
static_assert(Time(INT16_MIN) != kBadTime);
static_assert(Time(INT16_MAX + static_cast<int32_t>(1)) == kBadTime);
static_assert(Time(INT16_MIN - static_cast<int32_t>(1)) == kBadTime);

void nonContstexprTests() {
    TEST_ASSERT(4200 == Time::fromMillis(4200).toMillis());
    TEST_ASSERT(4200 == Time::fromMillis(4250).toMillis());
    TEST_ASSERT(4300 == Time::fromMillis(4251).toMillis());
    TEST_ASSERT(kMaxTime + 2_ts != kBadTime); // TODO should fail?

    TEST_ASSERT(84_ts == (2. * 42_ts));
    TEST_ASSERT(-84_ts == ((-2.) * 42_ts));
    TEST_ASSERT(1639_s * 2. == kBadTime);
    TEST_ASSERT(1638_s * 2. == 3276_s);
}

void checkStringFormat() {
    char buf[16];
    // accurate time works forcly before 20 secs
    Time(42).getFormatedTime(buf, false);
    TEST_ASSERT_EQUAL_STRING("4.2", buf);

    Time(202).getFormatedTime(buf, false);
    TEST_ASSERT_EQUAL_STRING("20", buf);

    Time(202).getFormatedTime(buf);
    TEST_ASSERT_EQUAL_STRING("20.2", buf);

    Time(200).getFormatedTime(buf);
    TEST_ASSERT_EQUAL_STRING("20", buf);

    Time(200).getFormatedTime(buf, true, true);
    TEST_ASSERT_EQUAL_STRING("20.0", buf);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(nonContstexprTests);
    RUN_TEST(checkStringFormat);
    UNITY_END();
}
