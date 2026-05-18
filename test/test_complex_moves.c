#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

void test_classic_forward_reverse_small_block(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 4);
    board_move_piece_to(b, PIECE_SMALL_BLOCK, 1, 4, 0, 4);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_classic_two_small_blocks_forward_reverse(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 4);
    board_move_piece_to_empty_cell(b, 2, 3);

    board_move_piece_to(b, PIECE_SMALL_BLOCK, 2, 4, 2, 3);
    board_move_piece_to(b, PIECE_SMALL_BLOCK, 1, 4, 0, 4);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_classic_other_small_block_forward_reverse(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 3, 4);
    board_move_piece_to(b, PIECE_SMALL_BLOCK, 2, 4, 3, 4);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_vertical_i_forward_reverse(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(1, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 0);
    board_move_piece_to(b, PIECE_VERTICAL_I, 0, 1, 0, 0);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_horizontal_i_forward_reverse(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 0);
    board_move_piece_to(b, PIECE_HORIZONTAL_I, 1, 0, 0, 0);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_big_square_forward_reverse(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 3, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 2, 4);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 1);
    board_move_piece_to(b, PIECE_BIG_SQUARE, 1, 1, 0, 1);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

void test_custom_board_three_moves_forward_reverse(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(0, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 0);
    b->num_vertical = 2;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 1, 1);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 2, 1);
    board_t orig = board_clone(b);

    board_move_piece_to_empty_cell(b, 0, 0);
    board_move_piece_to_empty_cell(b, 1, 0);
    board_move_piece_to_empty_cell(b, 2, 0);

    board_move_piece_to(b, PIECE_SMALL_BLOCK, 1, 0, 2, 0);
    board_move_piece_to(b, PIECE_SMALL_BLOCK, 0, 0, 1, 0);
    board_move_piece_to(b, PIECE_SMALL_BLOCK, 0, 1, 0, 0);

    TEST_ASSERT_TRUE(is_board_equal(&orig, b));
    destroy_board(b);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_classic_forward_reverse_small_block);
    RUN_TEST(test_classic_two_small_blocks_forward_reverse);
    RUN_TEST(test_classic_other_small_block_forward_reverse);
    RUN_TEST(test_vertical_i_forward_reverse);
    RUN_TEST(test_horizontal_i_forward_reverse);
    RUN_TEST(test_big_square_forward_reverse);
    RUN_TEST(test_custom_board_three_moves_forward_reverse);

    return UNITY_END();
}
