#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// Big square piece (2x2)
// =============================================================================

void test_is_position_free_of_big_square(void) {
    board_t *board = build_board();

    board->big_piece = 0;

    TEST_ASSERT_FALSE(is_position_free_of_big_square(board, 0, 0));
    TEST_ASSERT_FALSE(is_position_free_of_big_square(board, 1, 0));
    TEST_ASSERT_FALSE(is_position_free_of_big_square(board, 0, 1));
    TEST_ASSERT_FALSE(is_position_free_of_big_square(board, 1, 1));

    TEST_ASSERT_TRUE(is_position_free_of_big_square(board, 3, 4));

    destroy_board(board);
}

void test_does_big_square_fit_in(void) {
    board_t *board = build_board();
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 0, 0);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 3, 0);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 3, 4);

    TEST_ASSERT_TRUE(does_big_square_fit_in(board, 1, 2));

    board->big_piece = set_big_square_position(1, 2);
    TEST_ASSERT_FALSE(does_big_square_fit_in(board, 1, 2));

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

void test_set_big_piece_position(void) {
    TEST_ASSERT_EQUAL(set_big_square_position(0, 0), 0);
    TEST_ASSERT_EQUAL(set_big_square_position(1, 0), 1);
    TEST_ASSERT_EQUAL(set_big_square_position(2, 0), 2);
    TEST_ASSERT_EQUAL(set_big_square_position(0, 1), 3);
    TEST_ASSERT_EQUAL(set_big_square_position(1, 1), 4);
    TEST_ASSERT_EQUAL(set_big_square_position(2, 1), 5);
}

void test_get_big_piece_position_idempotency(void) {
    {
        const uint_fast16_t position = set_big_square_position(0, 0);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 0);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 0);
    }

    {
        const uint_fast16_t position = set_big_square_position(1, 0);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 1);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 0);
    }

    {
        const uint_fast16_t position = set_big_square_position(0, 1);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 0);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 1);
    }

    {
        const uint_fast16_t position = set_big_square_position(1, 1);
        TEST_ASSERT_EQUAL(get_x_position_from_big_square(position), 1);
        TEST_ASSERT_EQUAL(get_y_position_from_big_square(position), 1);
    }
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_is_position_free_of_big_square);
    RUN_TEST(test_does_big_square_fit_in);
    RUN_TEST(test_get_big_piece_position);
    RUN_TEST(test_set_big_piece_position);
    RUN_TEST(test_get_big_piece_position_idempotency);

    return UNITY_END();
}
