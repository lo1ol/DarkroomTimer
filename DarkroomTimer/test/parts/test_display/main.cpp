#include <Arduino.h>
#include <unity.h>

#include <Hardware.h>

void setUp() {
    gDisplay.reset();
    gCurrentTime = 0;
}

void tearDown() {}

void checkDisplay() {
    gDisplay.reset();
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(1));
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());

    gDisplay[0] << "kek";
    gDisplay[1] >> "kek";
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());

    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(1));
    gDisplay.tick();
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());
    TEST_ASSERT_EQUAL_STRING("kek             ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", gLcdWrap.getLine(1));

    gDisplay.reset();
    TEST_ASSERT_EQUAL_STRING("kek             ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", gLcdWrap.getLine(1));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrap.getLine(1));
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());
    gDisplay.tick();
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());

    gDisplay[0] << "kek";
    gDisplay.tick();
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
}

void checkDisplayLineGeneric() {
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2" >> "kek3";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2   kek3", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2 " << "kek3 " << "kek4 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3 k", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] << "kek1 " << "kek2 " << "kek3 " >> "kek4 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].reset();
    // clang-format off
    gDisplay[0] >> "kek4 " << "kek1 " << "kek2 " << "kek3 ";
    // clang-format on
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
}

void checkDisplayLineFastRepaint() {
    gDisplay[0] << "0123456789abcdef";
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));

    gDisplay[0].fastCurrentRepaint("kek");
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    // current is not set
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());

    gDisplay[0].reset();
    gDisplay[0] << "012";
    gDisplay[0].print("345", true);
    gDisplay[0] << "6789abcdef";
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    gDisplay[0].fastCurrentRepaint("kek");
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012kek6789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].fastCurrentRepaint("lol");
    TEST_ASSERT_EQUAL_STRING("012kek6789abcdef", gLcdWrap.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012lol6789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].restore();
    TEST_ASSERT_EQUAL_STRING("012lol6789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].fastCurrentRepaint("petr1");
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", gLcdWrap.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012pet6789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].fastCurrentRepaint("ha");
    TEST_ASSERT_EQUAL_STRING("012pet6789abcdef", gLcdWrap.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("012 ha6789abcdef", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
}

void checkDisplayLineBlink() {
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true);
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";

    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());

    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].fastCurrentRepaint("lol");
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].restore();
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());

    gDisplay[0].resetBlink(true);
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].fastCurrentRepaint("lol");
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].resetBlink(true);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].restore();
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));

    gLcdWrap.resetCallCount();

    gDisplay[0].reset();
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true, "ha");
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", gLcdWrap.getLine(0));
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());

    gDisplay[0].reset();
    gDisplay[0] << "lol";
    gDisplay[0].print("kek", true);
    gDisplay[0].resetBlink(true);
    gDisplay[0] << "cheburek";
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(2, gLcdWrap.printCallCount());
    gCurrentTime += 500;
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    gCurrentTime += 500;
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(1, gLcdWrap.printCallCount());
    gDisplay[0].tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", gLcdWrap.getLine(0));
    TEST_ASSERT_EQUAL(0, gLcdWrap.printCallCount());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkDisplay);
    RUN_TEST(checkDisplayLineGeneric);
    RUN_TEST(checkDisplayLineFastRepaint);
    RUN_TEST(checkDisplayLineBlink);
    UNITY_END();
}
