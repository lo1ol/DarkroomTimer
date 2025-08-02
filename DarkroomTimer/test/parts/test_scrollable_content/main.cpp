#include <Arduino.h>
#include <unity.h>

#include <ScrollableContent.h>

#include "../../VirtEnv.h"

void setUp() {
    gLcdWrapMock = LiquidCrystalWrapMock{};
    gDisplay = Display{ &gLcdWrapMock };
    gMillis = &mockMillis;

    gCurrentTime = 0;
    gSettings.lagTime = 0_ts;
    gTimer = Timer{};
}

void tearDown() {}

void checkScrollableContentGeneric() {
    gDisplay.reset();

    ScrollableContent sc;
    TEST_ASSERT(!sc.m_changed);
    TEST_ASSERT_EQUAL(0, sc.lineCnt());
    sc.print("kek1");
    TEST_ASSERT_EQUAL(1, sc.lineCnt());
    TEST_ASSERT(sc.m_changed);
    sc.print("kek2");
    TEST_ASSERT_EQUAL(1, sc.lineCnt());
    sc.print("kek3");
    TEST_ASSERT_EQUAL(1, sc.lineCnt());
    sc.print("kek4");
    TEST_ASSERT_EQUAL(2, sc.lineCnt());
    sc.print("kek5");
    TEST_ASSERT_EQUAL(2, sc.lineCnt());
    sc.print("kek6");
    TEST_ASSERT_EQUAL(2, sc.lineCnt());
    sc.print("kek7");
    TEST_ASSERT_EQUAL(3, sc.lineCnt());

    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.paint();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    sc.paint();
    gDisplay.tick();

    sc.reset();
    TEST_ASSERT_EQUAL(0, sc.lineCnt());
    gDisplay.tick();
    sc.paint();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.print("1234567890abcde");
    sc.print("1");
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("1234567890abcde ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("1               ", gLcdWrapMock.getLine(1));
    sc.reset();

    sc.print("1234567890abcd");
    sc.print("1");
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("1234567890abcd 1", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));
    sc.reset();
}

void checkScrollableContentScroll() {
    gDisplay.reset();

    ScrollableContent sc;
    TEST_ASSERT_EQUAL(0, sc.lineCnt());
    sc.print("kek1");
    sc.print("kek2");
    sc.print("kek3");
    sc.print("kek4");
    sc.print("kek5");
    sc.print("kek6");
    sc.print("kek7");
    sc.print("kek8");
    sc.print("kek9");
    sc.print("kek10");
    sc.print("kek11");
    sc.print("kek12");
    TEST_ASSERT_EQUAL(5, sc.lineCnt());

    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    gDisplay.tick();
    sc.scroll(1);
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek12           ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek12           ", gLcdWrapMock.getLine(1));

    sc.scroll(0);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek12           ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek10 kek11     ", gLcdWrapMock.getLine(1));

    sc.reset();
    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.reset();
    sc.print("kek1");
    sc.print("kek2");
    sc.print("kek3");
    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.print("kek4");
    sc.print("kek5");
    sc.print("kek6");
    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
}

void checkScrollableContentStartNewLine() {
    gDisplay.reset();

    ScrollableContent sc;
    TEST_ASSERT(!sc.m_changed);
    sc.startNewLine();
    TEST_ASSERT(sc.m_changed);
    TEST_ASSERT_EQUAL(0, sc.lineCnt());
    sc.startNewLine();
    TEST_ASSERT_EQUAL(0, sc.lineCnt());
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    sc.print("kek1");
    sc.print("kek2");
    sc.startNewLine();
    TEST_ASSERT_EQUAL(1, sc.lineCnt());
    sc.print("kek3");
    sc.startNewLine();
    TEST_ASSERT_EQUAL(2, sc.lineCnt());
    sc.startNewLine();
    TEST_ASSERT_EQUAL(2, sc.lineCnt());
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2       ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek3            ", gLcdWrapMock.getLine(1));

    sc.print("kek4");
    TEST_ASSERT_EQUAL(3, sc.lineCnt());

    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2       ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek3            ", gLcdWrapMock.getLine(1));

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek3            ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4            ", gLcdWrapMock.getLine(1));

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2       ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek3            ", gLcdWrapMock.getLine(1));
}

void checkScrollableContentCurrent() {
    gDisplay.reset();

    ScrollableContent sc;
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.paint();
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.print("kek1");
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.print("kek2");
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.paint();
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.reset();

    sc.print("kek1");
    sc.print("kek2", true);
    TEST_ASSERT(!sc.currentIsPrinted());
    sc.print("kek3");
    sc.print("kek4");
    sc.print("kek5");
    sc.print("kek6");
    sc.print("kek7");
    sc.print("kek8");
    sc.print("kek9");

    sc.paint();
    TEST_ASSERT(sc.currentIsPrinted());
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1      kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.paint();
    gDisplay.tick();

    gCurrentTime += 500;
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    gCurrentTime += 500;
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1      kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(!sc.currentIsPrinted());

    gCurrentTime += 500;
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(!sc.currentIsPrinted());

    gCurrentTime += 500;
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7 kek8 kek9  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(!sc.currentIsPrinted());

    sc.scroll(-1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1      kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    gCurrentTime += 500;
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1      kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.reset();
    sc.print("kek1");
    sc.print("kek2", true);
    sc.print("kek3");
    sc.print("kek4", true);
    sc.print("kek5");
    sc.print("kek6");
    sc.print("kek7");
    sc.print("kek8");
    sc.print("kek9");

    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("     kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.reset();
    sc.print("kek1");
    sc.print("kek2", true, "1");
    sc.print("kek3");
    sc.print("kek4", true, "2");
    sc.print("kek5");
    sc.print("kek6");
    sc.print("kek7");
    sc.print("kek8");
    sc.print("kek9");

    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("   2 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    gDisplay.resetBlink(false);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("   2 kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.reset();
    sc.print("kek1");
    sc.print("kek2", true, "1234");
    sc.print("kek3");
    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 1234 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.reset();
    sc.print("kek1");
    sc.print("kek2", true, "");
    sc.print("kek3");
    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1      kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT(sc.currentIsPrinted());

    sc.reset();
    sc.print("kek1");
    sc.print("kek2", true, "1");
    sc.print("kek3");
    sc.print("kek4", true);
    sc.print("kek5");
    sc.print("kek6");

    sc.paint();
    gDisplay.resetBlink(true);
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("     kek5 kek6  ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(sc.currentIsPrinted());

    // don't need a croll after printing current
    sc.reset();
    sc.print("kek1");
    sc.print("kek2");
    sc.print("kek3");
    sc.print("kek4");
    sc.print("kek5");
    sc.print("kek6");
    TEST_ASSERT(!sc.m_needGoToCurrent);
    sc.print("kek7", true);
    TEST_ASSERT(sc.m_needGoToCurrent);
    TEST_ASSERT(sc.m_changed);
    sc.print("kek8");
    sc.paint();
    TEST_ASSERT(!sc.m_needGoToCurrent);
    TEST_ASSERT(!sc.m_changed);
    gDisplay.tick();

    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("     kek8       ", gLcdWrapMock.getLine(1));
}

void checkScrollableContentTimer() {
    gDisplay.reset();
    ScrollableContent sc;

    gTimer.start(2_s);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    gCurrentTime += 500;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));
    gTimer.reset();

    gTimer.start(2_s);
    gTimer.tick();
    gCurrentTime += 500;
    gTimer.tick();
    sc.print("kek1");
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1            ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("                ", gLcdWrapMock.getLine(1));

    gTimer.reset();
    sc.reset();
    sc.print("kek1");
    sc.print("kek2");
    sc.print("kek3");
    sc.print("kek4");
    sc.print("kek5", true);
    sc.print("kek6");
    sc.print("kek7");
    gTimer.tick();
    gTimer.start(2_s);
    TEST_ASSERT(sc.m_changed);
    sc.paint();
    TEST_ASSERT(!sc.m_changed);
    gDisplay.tick();

    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  Lag kek6  ", gLcdWrapMock.getLine(1));

    gCurrentTime += 1;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  2.0 kek6  ", gLcdWrapMock.getLine(1));

    // can't scroll
    sc.scroll(1);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  2.0 kek6  ", gLcdWrapMock.getLine(1));

    gCurrentTime += 999;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  1.0 kek6  ", gLcdWrapMock.getLine(1));

    gCurrentTime += 1000;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4      kek6  ", gLcdWrapMock.getLine(1));

    gCurrentTime += 500;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    gTimer.tick();
    gTimer.start(2_s);
    gCurrentTime += 500;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  1.5 kek6  ", gLcdWrapMock.getLine(1));
    gTimer.stop();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4      kek6  ", gLcdWrapMock.getLine(1));

    gSettings.lagTime = 1_s;
    gTimer.tick();
    gTimer.start(2_s);
    gCurrentTime += 1000;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  Lag kek6  ", gLcdWrapMock.getLine(1));

    gCurrentTime += 1;
    gTimer.tick();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4  2.0 kek6  ", gLcdWrapMock.getLine(1));

    gTimer.reset();
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT_EQUAL_STRING("kek1 kek2 kek3  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));

    // check we auto scroll after gTimer started
    sc.reset();
    sc.print("kek1");
    sc.print("kek2");
    TEST_ASSERT(!sc.m_needGoToCurrent);
    sc.print("kek3", true);
    TEST_ASSERT(sc.m_needGoToCurrent);
    sc.print("kek4");
    sc.print("kek5");
    sc.print("kek6");
    sc.print("kek7");
    sc.paint();
    TEST_ASSERT(!sc.m_needGoToCurrent);
    TEST_ASSERT(!sc.m_changed);
    sc.scroll(1);
    sc.paint();
    gDisplay.tick();

    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek7            ", gLcdWrapMock.getLine(1));
    TEST_ASSERT(!sc.currentIsPrinted());

    gTimer.start(2_s);
    sc.paint();
    gDisplay.tick();
    TEST_ASSERT(!sc.m_needGoToCurrent);
    TEST_ASSERT(!sc.m_changed);
    TEST_ASSERT(sc.currentIsPrinted());

    TEST_ASSERT_EQUAL_STRING("kek1 kek2  Lag  ", gLcdWrapMock.getLine(0));
    TEST_ASSERT_EQUAL_STRING("kek4 kek5 kek6  ", gLcdWrapMock.getLine(1));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkScrollableContentGeneric);
    RUN_TEST(checkScrollableContentScroll);
    RUN_TEST(checkScrollableContentStartNewLine);
    RUN_TEST(checkScrollableContentCurrent);
    RUN_TEST(checkScrollableContentTimer);
    UNITY_END();
}
