#include <Arduino.h>
#include <unity.h>

#include <Display.h>
#include <Tools.h>

#include "../VirtEnv.h"

void setUp() {
    gLcdWrapMock = LiquidCrystalWrapMock{};
    gDisplay = Display{ &gLcdWrapMock };
    gMillis = &mockMillis;
    gCurrentTime = 0;
}

void tearDown() {}

void checkDisplay() {
    gDisplay.reset();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    gDisplay[0] << "kek";
    gDisplay[1] >> "kek";

    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek             ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", gLcdWrapMock.getLine(1));

    gDisplay.reset();
    TEST_ASSERT_EQUAL_STRING("kek             ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", gLcdWrapMock.getLine(1));
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));
    gDisplay.tick();
}

void checkDisplayLineGeneric() {
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2" >> "kek3";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2   kek3", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2 " << "kek3 " << "kek4 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3 k", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2 " << "kek3 " >> "kek4 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] >> "kek4 " << "kek1 " << "kek2 " << "kek3 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", gLcdWrapMock.getLine(0));
}

void checkDisplayLineFastRepaint() {
    gDisplay[0] << "0123456789abcdef";
    gDisplay[0].tick();

    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));

    gDisplay[0].fastCurrentRepaint("kek");
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    // current is not set
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    gDisplay[0] << "012";
    gDisplay[0].print("345", true);
    gDisplay[0] << "6789abcdef";
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));
    gDisplay[0].fastCurrentRepaint("kek");
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012kek6789abcdef", gLcdWrapMock.getLine(0));

    gDisplay[0].fastCurrentRepaint("lol");
    TEST_ASSERT_EQUAL_STRING("012kek6789abcdef", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012lol6789abcdef", gLcdWrapMock.getLine(0));

    gDisplay[0].restore();
    TEST_ASSERT_EQUAL_STRING("012lol6789abcdef", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrapMock.getLine(0));
}

void checkDisplayLineBlink() {
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true);
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";

    gDisplay[0].tick();

    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));

    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrapMock.getLine(0));

    gDisplay[0].fastCurrentRepaint("lol");
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].restore();
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrapMock.getLine(0));

    gDisplay[0].resetBlink(true);
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].fastCurrentRepaint("lol");
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].resetBlink(true);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].restore();
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true, "ha");
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", gLcdWrapMock.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrapMock.getLine(0));

    gDisplay[0].reset();
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true);
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));
    gCurrentTime += 500;
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrapMock.getLine(0));
    gCurrentTime += 500;
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrapMock.getLine(0));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkDisplay);
    RUN_TEST(checkDisplayLineGeneric);
    RUN_TEST(checkDisplayLineFastRepaint);
    RUN_TEST(checkDisplayLineBlink);
    UNITY_END();
}
