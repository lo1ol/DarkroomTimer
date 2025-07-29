#include <Arduino.h>
#include "unity.h"

#include <Tools.h>

void setUp() {}

void tearDown() {}

void checkAlignStr() {
    {
        char buf[16] = "2";
        alignStr(buf, 0);
        TEST_ASSERT_EQUAL_STRING("2", buf);
    }

    {
        char buf[16] = "2";
        alignStr(buf, 1);
        TEST_ASSERT_EQUAL_STRING("2", buf);
    }

    {
        char buf[16] = "2";
        alignStr(buf, 2);
        TEST_ASSERT_EQUAL_STRING(" 2", buf);
    }

    {
        char buf[16] = "2";
        alignStr(buf, 3);
        TEST_ASSERT_EQUAL_STRING("  2", buf);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkAlignStr);
    UNITY_END();
}
