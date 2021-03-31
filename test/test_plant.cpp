#include <unity.h>

#include <plant.h>

void test_plant_struct(void) {
  Plant plant{"AA:00:04:00:XX:YY"};
  TEST_ASSERT_EQUAL_STRING("AA:00:04:00:XX:YY", plant.macAddr);
  TEST_ASSERT_EQUAL(0, plant.metrics.temperature);
  TEST_ASSERT_EQUAL(0, plant.metrics.moisture);
  TEST_ASSERT_EQUAL(0, plant.metrics.light);
  TEST_ASSERT_EQUAL(0, plant.metrics.conductivity);
  TEST_ASSERT_EQUAL(0, plant.metrics.battery);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_plant_struct);
  return UNITY_END();
}