#include <Arduino.h>
#include <unity.h>

#include <Hardware.h>

void setUp() {
    gDisplay.reset();
    gCurrentTime = 0;
}

void tearDown() {}

void checkCreation() {
    auto anim = DisplayAnimation::createAnimation(DisplayAnimation::random);
    TEST_ASSERT_NOT_EQUAL(nullptr, anim);
    anim->~DisplayAnimation();

    anim = DisplayAnimation::createAnimation(DisplayAnimation::sleepyTimer);
    TEST_ASSERT_NOT_EQUAL(nullptr, anim);
    anim->~DisplayAnimation();

    anim = DisplayAnimation::createAnimation(DisplayAnimation::dickKicker);
    TEST_ASSERT_NOT_EQUAL(nullptr, anim);
    anim->~DisplayAnimation();

    anim = DisplayAnimation::createAnimation(DisplayAnimation::dvd);
    TEST_ASSERT_NOT_EQUAL(nullptr, anim);
    anim->~DisplayAnimation();
}

void testCommonAnimFunctions(DisplayAnimation::Id) {
    auto anim = DisplayAnimation::createAnimation(DisplayAnimation::dvd);
    TEST_ASSERT_NOT_EQUAL(nullptr, anim);

    for (uint8_t i = 0; i != 255; ++i) {
        gDisplay.reset();
        gDisplay.tick();
        TEST_ASSERT_TRUE(gLcd.empty());

        TEST_ASSERT_FALSE(gLcd.inFastPrint());
        gCurrentTime += anim->tick() + 1;
        TEST_ASSERT_FALSE(gLcd.inFastPrint());
        TEST_ASSERT_FALSE(gLcd.empty());
    }

    anim->~DisplayAnimation();
}

void checkDvd() {
    testCommonAnimFunctions(DisplayAnimation::dvd);
}

void checkDickKicker() {
    testCommonAnimFunctions(DisplayAnimation::dickKicker);
}

void checkSleepy() {
    testCommonAnimFunctions(DisplayAnimation::sleepyTimer);
}

void checkSpaceship() {
    testCommonAnimFunctions(DisplayAnimation::spaceship);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkCreation);
    RUN_TEST(checkDvd);
    RUN_TEST(checkDickKicker);
    RUN_TEST(checkSleepy);
    RUN_TEST(checkSpaceship);
    UNITY_END();
}
