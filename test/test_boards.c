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

// =============================================================================
// Vertical I pieces (2x1)
// =============================================================================

void test_get_vertical_i_position(void) {
    uint64_t pieces = 0;

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 0), 0);

    pieces = set_vertical_i_position(pieces, 0, 3, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 0), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 0), 3);

    pieces = set_vertical_i_position(pieces, 1, 1, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 1), 1);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 1), 2);

    pieces = set_vertical_i_position(pieces, 5, 0, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 5), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 5), 3);
}

void test_set_vertical_i_position(void) {
    uint64_t pieces = 0;

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(pieces, 0);

    pieces = set_vertical_i_position(pieces, 0, 1, 0);
    TEST_ASSERT_EQUAL(pieces, 1);

    pieces = set_vertical_i_position(pieces, 1, 0, 1);
    TEST_ASSERT_EQUAL(pieces, ((uint64_t)4 << 4) | 1);

    pieces = set_vertical_i_position(0, 2, 3, 2);
    TEST_ASSERT_EQUAL(pieces, (uint64_t)(3 + 2 * 4) << 8);
}

void test_set_and_get_vertical_i_idempotency(void) {
    uint64_t pieces = 0;

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 0), 0);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 0), 0);

    pieces = set_vertical_i_position(pieces, 1, 2, 1);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 1), 2);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 1), 1);

    pieces = set_vertical_i_position(pieces, 2, 3, 3);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 2), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 2), 3);

    pieces = set_vertical_i_position(pieces, 0, 3, 2);
    TEST_ASSERT_EQUAL(get_x_position_from_vertical_i(pieces, 0), 3);
    TEST_ASSERT_EQUAL(get_y_position_from_vertical_i(pieces, 0), 2);
}

void test_is_vertical_i_position_valid(void) {
    uint64_t pieces = 0;

    TEST_ASSERT_FALSE(is_vertical_i_position_valid(pieces, 2));

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    pieces = set_vertical_i_position(pieces, 1, 0, 2);
    TEST_ASSERT_TRUE(is_vertical_i_position_valid(pieces, 2));

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    pieces = set_vertical_i_position(pieces, 1, 0, 1);
    TEST_ASSERT_FALSE(is_vertical_i_position_valid(pieces, 2));

    pieces = set_vertical_i_position(pieces, 0, 0, 0);
    pieces = set_vertical_i_position(pieces, 1, 2, 0);
    TEST_ASSERT_TRUE(is_vertical_i_position_valid(pieces, 2));
}

void test_is_position_free_of_vertical_i(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->num_vertical = 1;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 2, 2);

    TEST_ASSERT_FALSE(is_position_free_of_vertical_i(board, 2, 2));
    TEST_ASSERT_FALSE(is_position_free_of_vertical_i(board, 2, 3));
    TEST_ASSERT_TRUE(is_position_free_of_vertical_i(board, 0, 0));
    TEST_ASSERT_TRUE(is_position_free_of_vertical_i(board, 3, 4));

    destroy_board(board);
}

void test_does_vertical_i_fit_in(void) {
    board_t *board = build_board();
    board->big_piece = set_big_square_position(0, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 1, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 2, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 3, 4);

    TEST_ASSERT_TRUE(does_vertical_i_fit_in(board, 0, 0));

    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);
    board->num_vertical = 1;
    TEST_ASSERT_FALSE(does_vertical_i_fit_in(board, 0, 0));

    board->big_piece = set_big_square_position(2, 1);
    TEST_ASSERT_FALSE(does_vertical_i_fit_in(board, 2, 2));
    TEST_ASSERT_TRUE(does_vertical_i_fit_in(board, 1, 0));

    destroy_board(board);
}

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

int main(void) {
	UNITY_BEGIN();

    // Lifecycle
    RUN_TEST(test_build_and_destroy);

    // Board
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

    // Big square piece (2x2)
    RUN_TEST(test_is_position_free_of_big_square);
    RUN_TEST(test_does_big_square_fit_in);
    RUN_TEST(test_get_big_piece_position);
    RUN_TEST(test_set_big_piece_position);
    RUN_TEST(test_get_big_piece_position_idempotency);

    // Small blocks (1x1)
    RUN_TEST(test_get_small_piece_position);
    RUN_TEST(test_set_small_piece_position);
    RUN_TEST(test_set_and_get_small_piece_idempotency);
    RUN_TEST(test_is_position_free_of_small_block);
    RUN_TEST(test_does_small_block_fit_in);
    RUN_TEST(test_is_small_block_position_valid);

    // Vertical I pieces (2x1)
    RUN_TEST(test_get_vertical_i_position);
    RUN_TEST(test_set_vertical_i_position);
    RUN_TEST(test_set_and_get_vertical_i_idempotency);
    RUN_TEST(test_is_vertical_i_position_valid);
    RUN_TEST(test_is_position_free_of_vertical_i);
    RUN_TEST(test_does_vertical_i_fit_in);

    // Horizontal I pieces (1x2)
    RUN_TEST(test_get_horizontal_i_position);
    RUN_TEST(test_set_horizontal_i_position);
    RUN_TEST(test_set_and_get_horizontal_i_idempotency);
    RUN_TEST(test_is_horizontal_i_position_valid);
    RUN_TEST(test_is_position_free_of_horizontal_i);
    RUN_TEST(test_does_horizontal_i_fit_in);

    // Cross-piece collision
    RUN_TEST(test_vertical_i_collides_with_horizontal_i);
    RUN_TEST(test_vertical_i_collides_with_small_block);
    RUN_TEST(test_horizontal_i_collides_with_small_block);

	return UNITY_END();
}
