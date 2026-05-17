#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

void test_build_and_destroy(void) {
    board_t *board = build_board();

    destroy_board(board);
}

void test_is_position_free(void) {
    board_t *board = build_board();
    board->big_piece = 0;

    TEST_ASSERT_TRUE(is_position_free(board, 0, 0));

    destroy_board(board);
}

void test_is_position_free_big_square(void) {
    board_t *board = build_board();

    board->big_piece = 0;

    TEST_ASSERT_FALSE(is_position_free_big_square(board, 0, 0));
    TEST_ASSERT_FALSE(is_position_free_big_square(board, 1, 0));
    TEST_ASSERT_FALSE(is_position_free_big_square(board, 0, 1));
    TEST_ASSERT_FALSE(is_position_free_big_square(board, 1, 1));

    TEST_ASSERT_TRUE(is_position_free_big_square(board, 3, 4));

    destroy_board(board);
}

void test_get_big_piece_position(void) {
    TEST_ASSERT_EQUAL(get_x_position_from_big_square(0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_big_square(0), 0);

    TEST_ASSERT_EQUAL(get_x_position_from_big_square(2), 2);
    TEST_ASSERT_EQUAL(get_y_position_from_big_square(2), 0);

    TEST_ASSERT_EQUAL(get_x_position_from_big_square(3), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_big_square(3), 1);
}

void test_encode_big_piece_position(void) {
    TEST_ASSERT_EQUAL(encode_big_square_position(0, 0), 0);
    TEST_ASSERT_EQUAL(encode_big_square_position(1, 0), 1);
    TEST_ASSERT_EQUAL(encode_big_square_position(2, 0), 2);
    TEST_ASSERT_EQUAL(encode_big_square_position(0, 1), 3);
    TEST_ASSERT_EQUAL(encode_big_square_position(1, 1), 4);
    TEST_ASSERT_EQUAL(encode_big_square_position(2, 1), 5);
}

void test_get_big_piece_position_idempotency(void) {
    {
        const uint_fast16_t position = encode_big_square_position(0, 0);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 0);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 0);
    }

    {
        const uint_fast16_t position = encode_big_square_position(1, 0);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 1);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 0);
    }

    {
        const uint_fast16_t position = encode_big_square_position(0, 1);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 0);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 1);
    }

    {
        const uint_fast16_t position = encode_big_square_position(1, 1);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 1);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 1);
    }
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_build_and_destroy);
    RUN_TEST(test_is_position_free);
    RUN_TEST(test_is_position_free_big_square);

    RUN_TEST(test_get_big_piece_position);
    RUN_TEST(test_encode_big_piece_position);
    RUN_TEST(test_get_big_piece_position_idempotency);

	return UNITY_END();
}
