#include <Arduino.h>
#include <unity.h>

#include <Utils.h>

void setUp() {}

void tearDown() {}

void checkAlignStr() {
    {
        char buf[16] = "abc";
        alignStr(buf, 0);
        TEST_ASSERT_EQUAL_STRING("", buf);
    }

    {
        char buf[16] = "abc";
        alignStr(buf, 1);
        TEST_ASSERT_EQUAL_STRING("a", buf);
    }

    {
        char buf[16] = "abc";
        alignStr(buf, 2);
        TEST_ASSERT_EQUAL_STRING("ab", buf);
    }

    {
        char buf[16] = "abc";
        alignStr(buf, 3);
        TEST_ASSERT_EQUAL_STRING("abc", buf);
    }

    {
        char buf[16] = "abc";
        alignStr(buf, 4);
        TEST_ASSERT_EQUAL_STRING(" abc", buf);
    }

    {
        char buf[16] = "abc";
        alignStr(buf, 5);
        TEST_ASSERT_EQUAL_STRING("  abc", buf);
    }
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(checkAlignStr);
    UNITY_END();
}
