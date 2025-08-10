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
static_assert(42_ts * int16_t(2) == 84_ts);
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

#define TEST_ASSERT_EQUAL_TIME(t1, t2) TEST_ASSERT_EQUAL((t1).toMillis(), (t2).toMillis());

void checkNonContstexprOps() {
    TEST_ASSERT_EQUAL_TIME(42_ts, Time::fromMillis(4200));
    TEST_ASSERT_EQUAL_TIME(42_ts, Time::fromMillis(4250));
    TEST_ASSERT_EQUAL_TIME(43_ts, Time::fromMillis(4251));
    TEST_ASSERT_EQUAL_TIME((kMaxTime + 1_ts), kBadTime);
    TEST_ASSERT_EQUAL_TIME((kMaxTime + 2_ts), kBadTime);
    TEST_ASSERT_EQUAL_TIME(((kMaxTime / 2 + 1_ts) * int16_t(2)), kBadTime);

    TEST_ASSERT_EQUAL_TIME(84_ts, (2. * 42_ts));
    TEST_ASSERT_EQUAL_TIME(-84_ts, ((-2.) * 42_ts));
    TEST_ASSERT_EQUAL_TIME(1639_s * 2., kBadTime);
    TEST_ASSERT_EQUAL_TIME(1638_s * 2., 3276_s);
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

void checkIds() {
    TEST_ASSERT_EQUAL(0, (-40_ts).getId());
    TEST_ASSERT_EQUAL(0, (0_ts).getId());
    TEST_ASSERT_EQUAL(5, (5_ts).getId());
    TEST_ASSERT_EQUAL(100, (100_ts).getId());
    TEST_ASSERT_EQUAL(100, (104_ts).getId());
    TEST_ASSERT_EQUAL(101, (105_ts).getId());
    TEST_ASSERT_EQUAL(120, (205_ts).getId());
    TEST_ASSERT_EQUAL(121, (210_ts).getId());
    TEST_ASSERT_EQUAL(200, (1000_ts).getId());
    TEST_ASSERT_EQUAL(200, (1040_ts).getId());
    TEST_ASSERT_EQUAL(201, (1050_ts).getId());
    TEST_ASSERT_EQUAL(220, (2000_ts).getId());
    TEST_ASSERT_EQUAL(220, (2099_ts).getId());
    TEST_ASSERT_EQUAL(221, (2100_ts).getId());
    TEST_ASSERT_EQUAL(300, (10000_ts).getId());
    TEST_ASSERT_EQUAL(300, (10499_ts).getId());
    TEST_ASSERT_EQUAL(301, (10500_ts).getId());
    TEST_ASSERT_EQUAL(316, (18000_ts).getId());
    TEST_ASSERT_EQUAL(316, kMaxTime.getId());
    TEST_ASSERT_EQUAL(0, kBadTime.getId());

    TEST_ASSERT_EQUAL_TIME(0_ts, Time::fromId(0));
    TEST_ASSERT_EQUAL_TIME(5_ts, Time::fromId(5));
    TEST_ASSERT_EQUAL_TIME(100_ts, Time::fromId(100));
    TEST_ASSERT_EQUAL_TIME(105_ts, Time::fromId(101));
    TEST_ASSERT_EQUAL_TIME(210_ts, Time::fromId(121));
    TEST_ASSERT_EQUAL_TIME(1000_ts, Time::fromId(200));
    TEST_ASSERT_EQUAL_TIME(1050_ts, Time::fromId(201));
    TEST_ASSERT_EQUAL_TIME(2000_ts, Time::fromId(220));
    TEST_ASSERT_EQUAL_TIME(2100_ts, Time::fromId(221));
    TEST_ASSERT_EQUAL_TIME(10000_ts, Time::fromId(300));
    TEST_ASSERT_EQUAL_TIME(10500_ts, Time::fromId(301));
    TEST_ASSERT_EQUAL_TIME(18000_ts, Time::fromId(316));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkNonContstexprOps);
    RUN_TEST(checkStringFormat);
    RUN_TEST(checkIds);
    UNITY_END();
}
