#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

// =============================================================================
// Small block (1x1)
// =============================================================================

void test_small_block_can_move_to_adjacent_empty(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 3, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 0);

    TEST_ASSERT_TRUE(can_piece_move(b, 1, 2));

    destroy_board(b);
}

void test_small_block_cannot_move_all_blocked(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 3, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 4;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 1);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 1, 0);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 2, 2, 2);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(can_piece_move(b, 1, 2));

    destroy_board(b);
}

void test_small_block_cannot_move_oob_negative(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 2, 4);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_small_block_cannot_move_oob_positive(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 3, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 3, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 0, 4);

    TEST_ASSERT_FALSE(can_piece_move(b, 3, 4));

    destroy_board(b);
}

// =============================================================================
// Vertical I (2x1)
// =============================================================================

void test_vertical_i_can_move_down(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_TRUE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_vertical_i_can_move_right(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_TRUE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_vertical_i_cannot_move_down_blocked(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_vertical_i_cannot_move_right_blocked(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_vertical_i_cannot_move_oob_negative(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_vertical_i_cannot_move_oob_positive(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 3, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 1, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 3, 3);

    TEST_ASSERT_FALSE(can_piece_move(b, 3, 3));

    destroy_board(b);
}

// =============================================================================
// Horizontal I (1x2)
// =============================================================================

void test_horizontal_i_can_move_right(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);

    TEST_ASSERT_TRUE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_horizontal_i_can_move_down(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);

    TEST_ASSERT_TRUE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_horizontal_i_cannot_move_right_blocked(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_horizontal_i_cannot_move_oob_negative(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_horizontal_i_cannot_move_oob_positive(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 3, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 1, 0);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 2, 4);

    TEST_ASSERT_FALSE(can_piece_move(b, 2, 4));

    destroy_board(b);
}

// =============================================================================
// Big square (2x2)
// =============================================================================

void test_big_square_can_move_down(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(1, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);

    TEST_ASSERT_TRUE(can_piece_move(b, 1, 1));

    destroy_board(b);
}

void test_big_square_cannot_move_all_blocked(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(1, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 1);

    TEST_ASSERT_FALSE(can_piece_move(b, 1, 1));

    destroy_board(b);
}

void test_big_square_cannot_move_oob_negative(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 2, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);

    TEST_ASSERT_FALSE(can_piece_move(b, 0, 0));

    destroy_board(b);
}

void test_big_square_cannot_move_oob_positive(void) {
    board_t *b = build_board();
    b->big_piece  = set_big_square_position(2, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 3, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 1, 3);

    TEST_ASSERT_FALSE(can_piece_move(b, 2, 3));

    destroy_board(b);
}

// =============================================================================
// Classic board
// =============================================================================

void test_classic_board_movable_pieces(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);

    TEST_ASSERT_TRUE(can_piece_move(b, 0, 4));
    TEST_ASSERT_TRUE(can_piece_move(b, 3, 4));
    TEST_ASSERT_FALSE(can_piece_move(b, 1, 0));

    destroy_board(b);
}

void test_classic_board_unmovable_pieces(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);

    TEST_ASSERT_FALSE(can_piece_move(b, 1, 0));
    TEST_ASSERT_FALSE(can_piece_move(b, 0, 1));
    TEST_ASSERT_FALSE(can_piece_move(b, 3, 0));
    TEST_ASSERT_FALSE(can_piece_move(b, 1, 2));

    destroy_board(b);
}

int main(void) {
    UNITY_BEGIN();

    // Small block (1x1)
    RUN_TEST(test_small_block_can_move_to_adjacent_empty);
    RUN_TEST(test_small_block_cannot_move_all_blocked);
    RUN_TEST(test_small_block_cannot_move_oob_negative);
    RUN_TEST(test_small_block_cannot_move_oob_positive);

    // Vertical I (2x1)
    RUN_TEST(test_vertical_i_can_move_down);
    RUN_TEST(test_vertical_i_can_move_right);
    RUN_TEST(test_vertical_i_cannot_move_down_blocked);
    RUN_TEST(test_vertical_i_cannot_move_right_blocked);
    RUN_TEST(test_vertical_i_cannot_move_oob_negative);
    RUN_TEST(test_vertical_i_cannot_move_oob_positive);

    // Horizontal I (1x2)
    RUN_TEST(test_horizontal_i_can_move_right);
    RUN_TEST(test_horizontal_i_can_move_down);
    RUN_TEST(test_horizontal_i_cannot_move_right_blocked);
    RUN_TEST(test_horizontal_i_cannot_move_oob_negative);
    RUN_TEST(test_horizontal_i_cannot_move_oob_positive);

    // Big square (2x2)
    RUN_TEST(test_big_square_can_move_down);
    RUN_TEST(test_big_square_cannot_move_all_blocked);
    RUN_TEST(test_big_square_cannot_move_oob_negative);
    RUN_TEST(test_big_square_cannot_move_oob_positive);

    // Classic board
    RUN_TEST(test_classic_board_movable_pieces);
    RUN_TEST(test_classic_board_unmovable_pieces);

    return UNITY_END();
}
