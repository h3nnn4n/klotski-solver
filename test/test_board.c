#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// Lifecycle
// =============================================================================

void test_build_and_destroy(void) {
    board_t *board = build_board();
    destroy_board(board);
}

// =============================================================================
// Board
// =============================================================================

void test_is_position_free(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    TEST_ASSERT_TRUE(is_position_free(board, 1, 4));

    destroy_board(board);
}

void test_classic_board_is_valid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    TEST_ASSERT_TRUE(is_board_valid(board));

    destroy_board(board);
}

void test_duplicate_small_blocks_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->small_blocks = set_small_block_position(board->small_blocks, 0, 1, 3);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_overlapping_vertical_i_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 3, 3, 0);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_overlapping_horizontal_i_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->num_horizontal = 2;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 1, 1, 2);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_small_block_on_big_square_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->small_blocks = set_small_block_position(board->small_blocks, 0, 1, 0);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_small_block_on_vertical_i_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->small_blocks = set_small_block_position(board->small_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_small_block_on_horizontal_i_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->small_blocks = set_small_block_position(board->small_blocks, 0, 1, 2);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_vertical_i_on_big_square_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 1, 0);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_horizontal_i_on_big_square_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_vertical_i_on_horizontal_i_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 1, 2);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_too_few_i_pieces_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->num_vertical = 3;

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_too_many_i_pieces_invalid(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    board->num_horizontal = 2;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 1, 1, 4);

    TEST_ASSERT_FALSE(is_board_valid(board));

    destroy_board(board);
}

void test_get_piece_at_empty(void) {
    board_t *board = build_board();

    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(board, 0, 0));
    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(board, 1, 0));
    TEST_ASSERT_EQUAL(PIECE_NONE, get_piece_at(board, 2, 0));
    TEST_ASSERT_EQUAL(PIECE_NONE, get_piece_at(board, 3, 4));

    destroy_board(board);
}

void test_get_piece_at_classic(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(board, 1, 0));
    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(board, 2, 1));
    TEST_ASSERT_EQUAL(PIECE_SMALL_BLOCK, get_piece_at(board, 0, 4));
    TEST_ASSERT_EQUAL(PIECE_SMALL_BLOCK, get_piece_at(board, 1, 3));
    TEST_ASSERT_EQUAL(PIECE_VERTICAL_I, get_piece_at(board, 0, 0));
    TEST_ASSERT_EQUAL(PIECE_VERTICAL_I, get_piece_at(board, 0, 1));
    TEST_ASSERT_EQUAL(PIECE_VERTICAL_I, get_piece_at(board, 3, 2));
    TEST_ASSERT_EQUAL(PIECE_HORIZONTAL_I, get_piece_at(board, 1, 2));
    TEST_ASSERT_EQUAL(PIECE_HORIZONTAL_I, get_piece_at(board, 2, 2));
    TEST_ASSERT_EQUAL(PIECE_NONE, get_piece_at(board, 1, 4));
    TEST_ASSERT_EQUAL(PIECE_NONE, get_piece_at(board, 2, 4));

    destroy_board(board);
}

// =============================================================================
// Cross-piece collision
// =============================================================================

void test_vertical_i_collides_with_horizontal_i(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(2, 3);

    board->num_horizontal = 1;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 1);

    TEST_ASSERT_FALSE(does_vertical_i_fit_in(board, 0, 0));
    TEST_ASSERT_TRUE(does_vertical_i_fit_in(board, 2, 0));

    board->num_vertical = 1;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(does_horizontal_i_fit_in(board, 0, 1));
    TEST_ASSERT_TRUE(does_horizontal_i_fit_in(board, 0, 3));

    destroy_board(board);
}

void test_vertical_i_collides_with_small_block(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 3, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(does_vertical_i_fit_in(board, 2, 1));
    TEST_ASSERT_TRUE(does_vertical_i_fit_in(board, 0, 0));

    destroy_board(board);
}

void test_horizontal_i_collides_with_small_block(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 3, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(does_horizontal_i_fit_in(board, 1, 2));
    TEST_ASSERT_TRUE(does_horizontal_i_fit_in(board, 0, 0));

    destroy_board(board);
}

// =============================================================================
// Solved
// =============================================================================

void test_classic_board_not_solved(void) {
    board_t *board = build_board();
    reset_board_to_classic(board);

    TEST_ASSERT_FALSE(is_board_solved(board));

    destroy_board(board);
}

void test_board_solved_when_big_square_at_bottom(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(1, 3);

    TEST_ASSERT_TRUE(is_board_solved(board));

    destroy_board(board);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_build_and_destroy);

    RUN_TEST(test_is_position_free);
    RUN_TEST(test_get_piece_at_empty);
    RUN_TEST(test_get_piece_at_classic);
    RUN_TEST(test_classic_board_is_valid);
    RUN_TEST(test_duplicate_small_blocks_invalid);
    RUN_TEST(test_overlapping_vertical_i_invalid);
    RUN_TEST(test_overlapping_horizontal_i_invalid);
    RUN_TEST(test_small_block_on_big_square_invalid);
    RUN_TEST(test_small_block_on_vertical_i_invalid);
    RUN_TEST(test_small_block_on_horizontal_i_invalid);
    RUN_TEST(test_vertical_i_on_big_square_invalid);
    RUN_TEST(test_horizontal_i_on_big_square_invalid);
    RUN_TEST(test_vertical_i_on_horizontal_i_invalid);
    RUN_TEST(test_too_few_i_pieces_invalid);
    RUN_TEST(test_too_many_i_pieces_invalid);

    RUN_TEST(test_vertical_i_collides_with_horizontal_i);
    RUN_TEST(test_vertical_i_collides_with_small_block);
    RUN_TEST(test_horizontal_i_collides_with_small_block);

    RUN_TEST(test_classic_board_not_solved);
    RUN_TEST(test_board_solved_when_big_square_at_bottom);

    return UNITY_END();
}
