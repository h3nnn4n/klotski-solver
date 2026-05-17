#include <unity.h>
#include "sample.h"

void setUp(void) {}
void tearDown(void) {}

void test_sample_init_counter_zero(void) {
	Sample s;
	sample_init(&s, 42u, 1u);
	TEST_ASSERT_EQUAL_INT(0, s.counter);
}

void test_sample_init_color_in_range(void) {
	Sample s;
	sample_init(&s, 42u, 1u);
	TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(0.0f, s.color[0]);
	TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, s.color[0]);
	TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(0.0f, s.color[1]);
	TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, s.color[1]);
	TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(0.0f, s.color[2]);
	TEST_ASSERT_LESS_OR_EQUAL_FLOAT(1.0f, s.color[2]);
}

void test_sample_tick_increments_counter(void) {
	Sample s;
	sample_init(&s, 42u, 1u);
	sample_tick(&s);
	TEST_ASSERT_EQUAL_INT(1, s.counter);
	sample_tick(&s);
	TEST_ASSERT_EQUAL_INT(2, s.counter);
}

void test_sample_reset_zeroes_counter(void) {
	Sample s;
	sample_init(&s, 42u, 1u);
	for (int i = 0; i < 10; i++) sample_tick(&s);
	sample_reset(&s);
	TEST_ASSERT_EQUAL_INT(0, s.counter);
}

void test_sample_reset_is_deterministic(void) {
	Sample s1, s2;
	sample_init(&s1, 42u, 1u);
	sample_init(&s2, 99u, 7u);
	sample_reset(&s1);
	sample_reset(&s2);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, s1.color[0], s2.color[0]);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, s1.color[1], s2.color[1]);
	TEST_ASSERT_FLOAT_WITHIN(1e-6f, s1.color[2], s2.color[2]);
}

void test_sample_color_changes_at_interval(void) {
	Sample s;
	sample_init(&s, 42u, 1u);
	float initial_r = s.color[0];
	for (int i = 0; i < SAMPLE_COLOR_CHANGE_INTERVAL; i++) sample_tick(&s);
	TEST_ASSERT_NOT_EQUAL(initial_r, s.color[0]);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_sample_init_counter_zero);
	RUN_TEST(test_sample_init_color_in_range);
	RUN_TEST(test_sample_tick_increments_counter);
	RUN_TEST(test_sample_reset_zeroes_counter);
	RUN_TEST(test_sample_reset_is_deterministic);
	RUN_TEST(test_sample_color_changes_at_interval);
	return UNITY_END();
}
