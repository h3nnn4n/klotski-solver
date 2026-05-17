#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

void test_board_move_piece_to_small_block(void) {
    board_t *b = build_board();
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 1, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 3, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);

    board_move_piece_to(b, PIECE_SMALL_BLOCK, 1, 2, 3, 1);

    uint_fast16_t sx = get_x_position_from_small_block(b->small_blocks, 0);
    uint_fast16_t sy = get_y_position_from_small_block(b->small_blocks, 0);
    TEST_ASSERT_EQUAL(3, sx);
    TEST_ASSERT_EQUAL(1, sy);

    destroy_board(b);
}

void test_board_move_piece_to_big_square(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(0, 0);

    board_move_piece_to(b, PIECE_BIG_SQUARE, 0, 0, 1, 1);

    TEST_ASSERT_EQUAL(set_big_square_position(1, 1), b->big_piece);

    destroy_board(b);
}

void test_board_move_piece_to_vertical_i(void) {
    board_t *b = build_board();
    b->num_vertical = 2;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 1, 2);

    board_move_piece_to(b, PIECE_VERTICAL_I, 0, 0, 2, 1);

    uint_fast16_t vx = get_x_position_from_vertical_i(b->vertical_blocks, 0);
    uint_fast16_t vy = get_y_position_from_vertical_i(b->vertical_blocks, 0);
    TEST_ASSERT_EQUAL(2, vx);
    TEST_ASSERT_EQUAL(1, vy);
    TEST_ASSERT_EQUAL(2, b->num_vertical);

    destroy_board(b);
}

void test_board_move_piece_to_horizontal_i(void) {
    board_t *b = build_board();
    b->num_horizontal = 1;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 1, 2);

    board_move_piece_to(b, PIECE_HORIZONTAL_I, 1, 2, 0, 0);

    uint_fast16_t hx = get_x_position_from_horizontal_i(b->horizontal_blocks, 0);
    uint_fast16_t hy = get_y_position_from_horizontal_i(b->horizontal_blocks, 0);
    TEST_ASSERT_EQUAL(0, hx);
    TEST_ASSERT_EQUAL(0, hy);

    destroy_board(b);
}

void test_move_piece_to_empty_cell_classic(void) {
    board_t *b = build_board();
    reset_board_to_classic(b);

    board_move_piece_to_empty_cell(b, 0, 4);

    uint_fast16_t sx = get_x_position_from_small_block(b->small_blocks, 0);
    uint_fast16_t sy = get_y_position_from_small_block(b->small_blocks, 0);
    TEST_ASSERT_EQUAL(1, sx);
    TEST_ASSERT_EQUAL(4, sy);

    destroy_board(b);
}

void test_move_piece_to_empty_cell_vertical_i(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(1, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);
    b->num_vertical = 1;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);

    board_move_piece_to_empty_cell(b, 0, 0);

    uint_fast16_t vx = get_x_position_from_vertical_i(b->vertical_blocks, 0);
    uint_fast16_t vy = get_y_position_from_vertical_i(b->vertical_blocks, 0);
    TEST_ASSERT_EQUAL(0, vx);
    TEST_ASSERT_EQUAL(1, vy);

    destroy_board(b);
}

void test_move_piece_to_empty_cell_big_square(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 3, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 2, 4);

    board_move_piece_to_empty_cell(b, 0, 1);

    TEST_ASSERT_EQUAL(set_big_square_position(1, 1), b->big_piece);

    destroy_board(b);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_board_move_piece_to_small_block);
    RUN_TEST(test_board_move_piece_to_big_square);
    RUN_TEST(test_board_move_piece_to_vertical_i);
    RUN_TEST(test_board_move_piece_to_horizontal_i);
    RUN_TEST(test_move_piece_to_empty_cell_classic);
    RUN_TEST(test_move_piece_to_empty_cell_vertical_i);
    RUN_TEST(test_move_piece_to_empty_cell_big_square);

    return UNITY_END();
}
