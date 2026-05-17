#include <stdint.h>
#include <unity.h>
#include "board.h"

void setUp(void) {}
void tearDown(void) {}

void test_board_clone_copies_all_fields(void) {
    board_t *orig = build_board();
    reset_board_to_classic(orig);

    board_t copy = board_clone(orig);

    TEST_ASSERT_EQUAL(orig->big_piece, copy.big_piece);
    TEST_ASSERT_EQUAL(orig->small_blocks, copy.small_blocks);
    TEST_ASSERT_EQUAL(orig->num_vertical, copy.num_vertical);
    TEST_ASSERT_EQUAL(orig->vertical_blocks, copy.vertical_blocks);
    TEST_ASSERT_EQUAL(orig->num_horizontal, copy.num_horizontal);
    TEST_ASSERT_EQUAL(orig->horizontal_blocks, copy.horizontal_blocks);

    destroy_board(orig);
}

void test_board_clone_is_deep_copy(void) {
    board_t *orig = build_board();
    reset_board_to_classic(orig);

    board_t copy = board_clone(orig);
    copy.big_piece = set_big_square_position(2, 2);
    TEST_ASSERT_NOT_EQUAL(orig->big_piece, copy.big_piece);

    destroy_board(orig);
}

void test_board_remove_big_square(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(2, 2);

    uint_fast16_t px, py;
    board_remove_big_square(b, &px, &py);

    TEST_ASSERT_EQUAL(2, px);
    TEST_ASSERT_EQUAL(2, py);
    TEST_ASSERT_EQUAL(15, b->big_piece);

    destroy_board(b);
}

void test_board_remove_small_block(void) {
    board_t *b = build_board();
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 0, 4);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 1, 3);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 2, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 3, 4);

    uint_fast16_t px, py;
    board_remove_small_block(b, 2, 2, &px, &py);

    TEST_ASSERT_EQUAL(2, px);
    TEST_ASSERT_EQUAL(2, py);

    uint_fast16_t sx = get_x_position_from_small_block(b->small_blocks, 2);
    uint_fast16_t sy = get_y_position_from_small_block(b->small_blocks, 2);
    TEST_ASSERT_FALSE(sx == 2 && sy == 2);

    destroy_board(b);
}

void test_board_remove_vertical_i_first(void) {
    board_t *b = build_board();
    b->num_vertical = 3;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 2, 1);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 2, 3, 3);

    uint_fast16_t px, py;
    board_remove_vertical_i(b, 0, 0, &px, &py);

    TEST_ASSERT_EQUAL(0, px);
    TEST_ASSERT_EQUAL(0, py);
    TEST_ASSERT_EQUAL(2, b->num_vertical);

    uint_fast16_t vx = get_x_position_from_vertical_i(b->vertical_blocks, 0);
    uint_fast16_t vy = get_y_position_from_vertical_i(b->vertical_blocks, 0);
    TEST_ASSERT_EQUAL(3, vx);
    TEST_ASSERT_EQUAL(3, vy);

    destroy_board(b);
}

void test_board_remove_vertical_i_last(void) {
    board_t *b = build_board();
    b->num_vertical = 3;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 0, 0);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 2, 1);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 2, 3, 3);

    uint_fast16_t px, py;
    board_remove_vertical_i(b, 3, 3, &px, &py);

    TEST_ASSERT_EQUAL(3, px);
    TEST_ASSERT_EQUAL(3, py);
    TEST_ASSERT_EQUAL(2, b->num_vertical);

    uint_fast16_t vx = get_x_position_from_vertical_i(b->vertical_blocks, 0);
    uint_fast16_t vy = get_y_position_from_vertical_i(b->vertical_blocks, 0);
    TEST_ASSERT_EQUAL(0, vx);
    TEST_ASSERT_EQUAL(0, vy);

    destroy_board(b);
}

void test_board_remove_horizontal_i(void) {
    board_t *b = build_board();
    b->num_horizontal = 2;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 0);
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 1, 1, 2);

    uint_fast16_t px, py;
    board_remove_horizontal_i(b, 1, 2, &px, &py);

    TEST_ASSERT_EQUAL(1, px);
    TEST_ASSERT_EQUAL(2, py);
    TEST_ASSERT_EQUAL(1, b->num_horizontal);

    uint_fast16_t hx = get_x_position_from_horizontal_i(b->horizontal_blocks, 0);
    uint_fast16_t hy = get_y_position_from_horizontal_i(b->horizontal_blocks, 0);
    TEST_ASSERT_EQUAL(0, hx);
    TEST_ASSERT_EQUAL(0, hy);

    destroy_board(b);
}

void test_board_remove_piece_big_square(void) {
    board_t *b = build_board();
    b->big_piece = set_big_square_position(1, 1);

    uint_fast16_t px, py;
    board_remove_piece(b, PIECE_BIG_SQUARE, 1, 1, &px, &py);

    TEST_ASSERT_EQUAL(1, px);
    TEST_ASSERT_EQUAL(1, py);
    TEST_ASSERT_EQUAL(15, b->big_piece);

    destroy_board(b);
}

void test_board_remove_piece_small_block(void) {
    board_t *b = build_board();
    b->small_blocks = set_small_block_position(b->small_blocks, 0, 3, 0);
    b->small_blocks = set_small_block_position(b->small_blocks, 1, 0, 2);
    b->small_blocks = set_small_block_position(b->small_blocks, 2, 0, 1);
    b->small_blocks = set_small_block_position(b->small_blocks, 3, 0, 0);

    uint_fast16_t px, py;
    board_remove_piece(b, PIECE_SMALL_BLOCK, 0, 2, &px, &py);

    TEST_ASSERT_EQUAL(0, px);
    TEST_ASSERT_EQUAL(2, py);

    destroy_board(b);
}

void test_board_remove_piece_vertical_i(void) {
    board_t *b = build_board();
    b->num_vertical = 2;
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 0, 1, 0);
    b->vertical_blocks = set_vertical_i_position(b->vertical_blocks, 1, 3, 2);

    uint_fast16_t px, py;
    board_remove_piece(b, PIECE_VERTICAL_I, 1, 1, &px, &py);

    TEST_ASSERT_EQUAL(1, px);
    TEST_ASSERT_EQUAL(0, py);
    TEST_ASSERT_EQUAL(1, b->num_vertical);

    destroy_board(b);
}

void test_board_remove_piece_horizontal_i(void) {
    board_t *b = build_board();
    b->num_horizontal = 2;
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 0, 0, 2);
    b->horizontal_blocks = set_horizontal_i_position(b->horizontal_blocks, 1, 2, 4);

    uint_fast16_t px, py;
    board_remove_piece(b, PIECE_HORIZONTAL_I, 3, 4, &px, &py);

    TEST_ASSERT_EQUAL(2, px);
    TEST_ASSERT_EQUAL(4, py);
    TEST_ASSERT_EQUAL(1, b->num_horizontal);

    destroy_board(b);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_board_clone_copies_all_fields);
    RUN_TEST(test_board_clone_is_deep_copy);
    RUN_TEST(test_board_remove_big_square);
    RUN_TEST(test_board_remove_small_block);
    RUN_TEST(test_board_remove_vertical_i_first);
    RUN_TEST(test_board_remove_vertical_i_last);
    RUN_TEST(test_board_remove_horizontal_i);
    RUN_TEST(test_board_remove_piece_big_square);
    RUN_TEST(test_board_remove_piece_small_block);
    RUN_TEST(test_board_remove_piece_vertical_i);
    RUN_TEST(test_board_remove_piece_horizontal_i);

    return UNITY_END();
}
