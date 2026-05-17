#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// Small blocks (1x1)
// =============================================================================

void test_get_small_piece_position(void) {
    uint64_t pieces = 0;

    pieces = set_small_block_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 0), 0);

    pieces = set_small_block_position(pieces, 0, 3, 4);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 0), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 0), 4);

    pieces = set_small_block_position(pieces, 1, 1, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 1), 1);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 1), 2);

    pieces = set_small_block_position(pieces, 3, 0, 4);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 3), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 3), 4);
}

void test_set_small_piece_position(void) {
    uint64_t pieces = 0;

    pieces = set_small_block_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(pieces, 0);

    pieces = set_small_block_position(pieces, 0, 1, 0);
    TEST_ASSERT_EQUAL(pieces, 1);

    pieces = set_small_block_position(pieces, 1, 0, 1);
    TEST_ASSERT_EQUAL(pieces, (4 << 5) | 1);

    pieces = set_small_block_position(0, 2, 3, 4);
    TEST_ASSERT_EQUAL(pieces, (3 + 4 * 4) << 10);
}

void test_set_and_get_small_piece_idempotency(void) {
    uint64_t pieces = 0;

    pieces = set_small_block_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 0), 0);

    pieces = set_small_block_position(pieces, 1, 2, 1);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 1), 2);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 1), 1);

    pieces = set_small_block_position(pieces, 2, 3, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 2), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 2), 3);

    pieces = set_small_block_position(pieces, 3, 0, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 3), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 3), 2);

    pieces = set_small_block_position(pieces, 0, 3, 4);
    TEST_ASSERT_EQUAL(get_x_position_from_small_block(pieces, 0), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_small_block(pieces, 0), 4);
}

void test_is_position_free_of_small_block(void) {
    board_t *board = build_board();
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 3, 0);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(is_position_free_of_small_block(board, 2, 2));
    TEST_ASSERT_FALSE(is_position_free_of_small_block(board, 0, 4));
    TEST_ASSERT_TRUE(is_position_free_of_small_block(board, 0, 0));
    TEST_ASSERT_TRUE(is_position_free_of_small_block(board, 1, 2));

    destroy_board(board);
}

void test_does_small_block_fit_in(void) {
    board_t *board = build_board();
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 3, 0);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(does_small_block_fit_in(board, 2, 2));
    TEST_ASSERT_TRUE(does_small_block_fit_in(board, 0, 2));

    destroy_board(board);
}

void test_is_small_block_position_valid(void) {
    uint_fast16_t position = 0;
    TEST_ASSERT_FALSE(is_small_block_position_valid(position));

    position = set_small_block_position(position, 0, 1, 1);
    position = set_small_block_position(position, 1, 2, 1);
    position = set_small_block_position(position, 2, 1, 2);
    position = set_small_block_position(position, 3, 2, 2);
    TEST_ASSERT_TRUE(is_small_block_position_valid(position));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_get_small_piece_position);
    RUN_TEST(test_set_small_piece_position);
    RUN_TEST(test_set_and_get_small_piece_idempotency);
    RUN_TEST(test_is_position_free_of_small_block);
    RUN_TEST(test_does_small_block_fit_in);
    RUN_TEST(test_is_small_block_position_valid);

    return UNITY_END();
}
