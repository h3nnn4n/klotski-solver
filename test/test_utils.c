#include <unity.h>
#include "utils.h"

void setUp(void) {}
void tearDown(void) {}

void test_lerp_midpoint(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, lerp(0.0f, 1.0f, 0.5f));
}

void test_lerp_t0(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 3.0f, lerp(3.0f, 7.0f, 0.0f));
}

void test_lerp_t1(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 7.0f, lerp(3.0f, 7.0f, 1.0f));
}

void test_lerp_negative(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, -5.0f, lerp(-10.0f, 0.0f, 0.5f));
}

void test_clampf_below(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, clampf(-1.0f, 0.0f, 1.0f));
}

void test_clampf_above(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, clampf(2.0f, 0.0f, 1.0f));
}

void test_clampf_in_range(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, clampf(0.5f, 0.0f, 1.0f));
}

void test_clampf_at_lo(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.0f, clampf(0.0f, 0.0f, 1.0f));
}

void test_clampf_at_hi(void) {
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, 1.0f, clampf(1.0f, 0.0f, 1.0f));
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_lerp_midpoint);
	RUN_TEST(test_lerp_t0);
	RUN_TEST(test_lerp_t1);
	RUN_TEST(test_lerp_negative);
	RUN_TEST(test_clampf_below);
	RUN_TEST(test_clampf_above);
	RUN_TEST(test_clampf_in_range);
	RUN_TEST(test_clampf_at_lo);
	RUN_TEST(test_clampf_at_hi);
	return UNITY_END();
}
