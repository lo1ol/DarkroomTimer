#include <Arduino.h>
#include <unity.h>

#include <common/Time.h>

void setUp() {}

void tearDown() {}

// constexpr tests
static_assert(Time(42).secs() == 4);
static_assert(Time(42).tenth() == 2);

static_assert(42_ts + 15_ts == 57_ts);
static_assert(42_ts - 15_ts == 27_ts);
static_assert(42_ts - 43_ts == kBadTime);
static_assert(42_ts * uint16_t(2) == 84_ts);
static_assert(42_ts / 5 == 8_ts);
static_assert(42_ts > 41_ts);
static_assert(42_ts < 43_ts);
static_assert(42_ts == 42_ts);

static_assert(Time(UINT16_MAX) == kBadTime);
static_assert(Time(UINT16_MAX - 1) == kBadTime);
static_assert(kMaxTime != kBadTime);
static_assert(kMaxTime - 1_ts != kBadTime);
static_assert(kMaxTime + 1_ts == kBadTime);
static_assert(Time(0) != kBadTime);
static_assert(Time(UINT16_MAX + static_cast<uint32_t>(1)) == kBadTime);
static_assert(Time(-1) == kBadTime);
static_assert((kMaxTime / 2 + 1_ts) * uint16_t(2) == kBadTime);

static_assert(84_ts == 2. * 42_ts);
static_assert(3277_s * 2. == kBadTime);
static_assert(3276_s * 2. == 6552_s);

static_assert(42_ts == Time::fromMillis(4200));
static_assert(42_ts == Time::fromMillis(4250));
static_assert(43_ts == Time::fromMillis(4251));

static_assert((42_ts).toMillis() == 4200);
static_assert((43_ts).toMillis() == 4300);

#define TEST_ASSERT_EQUAL_TIME(t1, t2) TEST_ASSERT_EQUAL((t1).toMillis(), (t2).toMillis());

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
    TEST_ASSERT_EQUAL(320, (20000_ts).getId());
    TEST_ASSERT_EQUAL(320, (20999_ts).getId());
    TEST_ASSERT_EQUAL(321, (21000_ts).getId());
    TEST_ASSERT_EQUAL(336, (36000_ts).getId());
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
    TEST_ASSERT_EQUAL_TIME(20000_ts, Time::fromId(320));
    TEST_ASSERT_EQUAL_TIME(21000_ts, Time::fromId(321));
    TEST_ASSERT_EQUAL_TIME(36000_ts, Time::fromId(380));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkStringFormat);
    RUN_TEST(checkIds);
    UNITY_END();
}
