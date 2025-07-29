#include <Arduino.h>
#include "unity.h"

void setUp() {
}

void tearDown() {
}

void fakeStaticTest() {
    /*
  static_assert(Time(42).secs() == 4);
  static_assert(Time(42).tenth() == 2);

  static_assert(Time(42) + Time(15) == Time(57));
  static_assert(Time(42) - Time(15) == Time(27));
  */
  TEST_ASSERT_TRUE(0);
}
void fakeStaticTest2() {
    /*
  static_assert(Time(42).secs() == 4);
  static_assert(Time(42).tenth() == 2);

  static_assert(Time(42) + Time(15) == Time(57));
  static_assert(Time(42) - Time(15) == Time(27));
  */
  TEST_ASSERT_TRUE(1);
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(fakeStaticTest);
  RUN_TEST(fakeStaticTest2);
  UNITY_END();
}
