#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// Horizontal I pieces (1x2)
// =============================================================================

void test_get_horizontal_i_position(void) {
    uint64_t pieces = 0;

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 0), 0);

    pieces = set_horizontal_i_position(pieces, 0, 2, 4);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 0), 2);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 0), 4);

    pieces = set_horizontal_i_position(pieces, 1, 1, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 1), 1);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 1), 2);

    pieces = set_horizontal_i_position(pieces, 3, 0, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 3), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 3), 3);
}

void test_set_horizontal_i_position(void) {
    uint64_t pieces = 0;

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(pieces, 0);

    pieces = set_horizontal_i_position(pieces, 0, 1, 0);
    TEST_ASSERT_EQUAL(pieces, 1);

    pieces = set_horizontal_i_position(pieces, 1, 0, 1);
    TEST_ASSERT_EQUAL(pieces, ((uint64_t)3 << 4) | 1);

    pieces = set_horizontal_i_position(0, 2, 2, 4);
    TEST_ASSERT_EQUAL(pieces, (uint64_t)(2 + 4 * 3) << 8);
}

void test_set_and_get_horizontal_i_idempotency(void) {
    uint64_t pieces = 0;

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 0), 0);

    pieces = set_horizontal_i_position(pieces, 1, 2, 1);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 1), 2);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 1), 1);

    pieces = set_horizontal_i_position(pieces, 2, 0, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 2), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 2), 3);

    pieces = set_horizontal_i_position(pieces, 0, 1, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_horizontal_i(pieces, 0), 1);
    TEST_ASSERT_EQUAL(get_y_position_from_horizontal_i(pieces, 0), 2);
}

void test_is_horizontal_i_position_valid(void) {
    uint64_t pieces = 0;

    TEST_ASSERT_FALSE(is_horizontal_i_position_valid(pieces, 2));

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    pieces = set_horizontal_i_position(pieces, 1, 2, 0);
    TEST_ASSERT_TRUE(is_horizontal_i_position_valid(pieces, 2));

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    pieces = set_horizontal_i_position(pieces, 1, 1, 0);
    TEST_ASSERT_FALSE(is_horizontal_i_position_valid(pieces, 2));

    pieces = set_horizontal_i_position(pieces, 0, 0, 0);
    pieces = set_horizontal_i_position(pieces, 1, 0, 1);
    TEST_ASSERT_TRUE(is_horizontal_i_position_valid(pieces, 2));
}

void test_is_position_free_of_horizontal_i(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->num_horizontal = 1;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 1, 2);

    TEST_ASSERT_FALSE(is_position_free_of_horizontal_i(board, 1, 2));
    TEST_ASSERT_FALSE(is_position_free_of_horizontal_i(board, 2, 2));
    TEST_ASSERT_TRUE(is_position_free_of_horizontal_i(board, 0, 0));
    TEST_ASSERT_TRUE(is_position_free_of_horizontal_i(board, 3, 4));

    destroy_board(board);
}

void test_does_horizontal_i_fit_in(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 1, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 2, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 3, 4);

    TEST_ASSERT_TRUE(does_horizontal_i_fit_in(board, 0, 0));

    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 0);
    board->num_horizontal = 1;
    TEST_ASSERT_FALSE(does_horizontal_i_fit_in(board, 0, 0));

    board->big_piece = set_big_square_position(0, 3);
    TEST_ASSERT_FALSE(does_horizontal_i_fit_in(board, 0, 3));
    TEST_ASSERT_TRUE(does_horizontal_i_fit_in(board, 0, 2));

    destroy_board(board);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_get_horizontal_i_position);
    RUN_TEST(test_set_horizontal_i_position);
    RUN_TEST(test_set_and_get_horizontal_i_idempotency);
    RUN_TEST(test_is_horizontal_i_position_valid);
    RUN_TEST(test_is_position_free_of_horizontal_i);
    RUN_TEST(test_does_horizontal_i_fit_in);

    return UNITY_END();
}
