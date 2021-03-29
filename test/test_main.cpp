#include <unity.h>

void test_generic(void) {
    TEST_ASSERT_EQUAL(1, 1);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_generic);
    return UNITY_END();
}
