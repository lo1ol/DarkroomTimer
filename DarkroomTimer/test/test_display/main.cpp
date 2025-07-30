#include <Arduino.h>
#include "unity.h"

#include <Display.h>

#include "../LiquidCrystalWrapMock.h"

void setUp() {}

void tearDown() {}

void checkDisplay() {
    LiquidCrystalWrapMock lcdWrapMock;
    Display d{ &lcdWrapMock };
    d.reset();
    d.tick();
    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(1));

    d[0] << "kek";
    d[1] >> "kek";

    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(1));
    d.tick();
    TEST_ASSERT_EQUAL_STRING("kek             ", lcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", lcdWrapMock.getLine(1));

    d.reset();
    TEST_ASSERT_EQUAL_STRING("kek             ", lcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("             kek", lcdWrapMock.getLine(1));
    d.tick();
    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", lcdWrapMock.getLine(1));
    d.tick();
}

void checkDisplayLineGeneric() {
    LiquidCrystalWrapMock lcdWrapMock;
    DisplayLine dl{ 0, &lcdWrapMock };
    // clang-format off
    dl << "kek1 " << "kek2" >> "kek3";
    // clang-format on
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2   kek3", lcdWrapMock.getLine(0));

    dl.reset();
    // clang-format off
    dl << "kek1 " << "kek2 " << "kek3 " << "kek4 ";
    // clang-format on
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3 k", lcdWrapMock.getLine(0));

    dl.reset();
    // clang-format off
    dl << "kek1 " << "kek2 " << "kek3 " >> "kek4 ";
    // clang-format on
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", lcdWrapMock.getLine(0));

    dl.reset();
    // clang-format off
    dl >> "kek4 " << "kek1 " << "kek2 " << "kek3 ";
    // clang-format on
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kkek4 ", lcdWrapMock.getLine(0));
}

void checkDisplayLineFastRepaint() {
    LiquidCrystalWrapMock lcdWrapMock;
    DisplayLine dl{ 0, &lcdWrapMock };
    dl.reset();
    dl << "0123456789abcdef";
    dl.tick();

    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", lcdWrapMock.getLine(0));

    dl.fastRepaint("kek", 3);
    TEST_ASSERT_EQUAL_STRING("012kek6789abcdef", lcdWrapMock.getLine(0));

    dl.fastRepaint("lol", 9);
    TEST_ASSERT_EQUAL_STRING("012kek678lolcdef", lcdWrapMock.getLine(0));

    dl.restore();
    TEST_ASSERT_EQUAL_STRING("012kek678lolcdef", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("0123456789abcdef", lcdWrapMock.getLine(0));
}

void checkDisplayLineBlink() {
    LiquidCrystalWrapMock lcdWrapMock;
    DisplayLine dl{ 0, &lcdWrapMock };
    dl.reset();
    dl << "lol";
    dl.print("kek", true);
    dl.resetBlink(true);
    dl << "cheburek";

    dl.tick();

    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", lcdWrapMock.getLine(0));

    dl.resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", lcdWrapMock.getLine(0));

    dl.fastRepaint("lol", 3);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.restore();
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", lcdWrapMock.getLine(0));

    dl.resetBlink(true);
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", lcdWrapMock.getLine(0));
    dl.fastRepaint("lol", 3);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.resetBlink(true);
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lollolcheburek  ", lcdWrapMock.getLine(0));
    dl.restore();
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lol   cheburek  ", lcdWrapMock.getLine(0));

    dl.reset();
    dl << "lol";
    dl.print("kek", true, "ha");
    dl.resetBlink(true);
    dl << "cheburek";
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", lcdWrapMock.getLine(0));
    dl.resetBlink(false);
    TEST_ASSERT_EQUAL_STRING("lol hacheburek  ", lcdWrapMock.getLine(0));
    dl.tick();
    TEST_ASSERT_EQUAL_STRING("lolkekcheburek  ", lcdWrapMock.getLine(0));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkDisplay);
    RUN_TEST(checkDisplayLineGeneric);
    RUN_TEST(checkDisplayLineFastRepaint);
    RUN_TEST(checkDisplayLineBlink);
    UNITY_END();
}
