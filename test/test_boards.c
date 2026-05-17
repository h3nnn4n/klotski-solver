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

void test_is_position_free_small_block(void) {
    board_t *board = build_board();
    board->big_piece = 0;
    board->small_blocks = 0;

    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);

    TEST_ASSERT_FALSE(is_position_free_small_block(board, 2, 2, 0));
    TEST_ASSERT_TRUE(is_position_free_small_block(board, 2, 2, 1));

    TEST_ASSERT_FALSE(is_position_free_small_block(board, 0, 0, 0));

    destroy_board(board);
}

void test_encode_vertical_i_position(void) {
    // Vertical I: valid x in [0,3], y in [0,3], encoded as x + y*4
    TEST_ASSERT_EQUAL(0,  encode_vertical_i_position(0, 0));
    TEST_ASSERT_EQUAL(3,  encode_vertical_i_position(3, 0));
    TEST_ASSERT_EQUAL(4,  encode_vertical_i_position(0, 1));
    TEST_ASSERT_EQUAL(7,  encode_vertical_i_position(3, 1));
    TEST_ASSERT_EQUAL(15, encode_vertical_i_position(3, 3));
}

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

void test_encode_horizontal_i_position(void) {
    // Horizontal I: valid x in [0,2], y in [0,4], encoded as x + y*3
    TEST_ASSERT_EQUAL(0,  encode_horizontal_i_position(0, 0));
    TEST_ASSERT_EQUAL(2,  encode_horizontal_i_position(2, 0));
    TEST_ASSERT_EQUAL(3,  encode_horizontal_i_position(0, 1));
    TEST_ASSERT_EQUAL(5,  encode_horizontal_i_position(2, 1));
    TEST_ASSERT_EQUAL(14, encode_horizontal_i_position(2, 4));
}

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

void test_is_position_free_vertical_i(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(0, 3);
    board->num_vertical = 1;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 2, 2);

    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 2, 2, 0));

    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 0, 0, 0));

    board->big_piece = encode_big_square_position(2, 1);
    TEST_ASSERT_FALSE(is_position_free_vertical_i(board, 2, 2, 0));

    destroy_board(board);
}

void test_is_position_free_vertical_i_collision_with_other(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(2, 3);
    board->num_vertical = 2;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 1, 0, 2);

    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 0, 0, 0));

    TEST_ASSERT_FALSE(is_position_free_vertical_i(board, 0, 2, 0));

    TEST_ASSERT_FALSE(is_position_free_vertical_i(board, 0, 1, 0));

    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 0, 3, 1));

    destroy_board(board);
}

void test_is_position_free_horizontal_i(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(0, 3);
    board->num_horizontal = 1;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 2, 2);

    TEST_ASSERT_TRUE(is_position_free_horizontal_i(board, 2, 2, 0));

    TEST_ASSERT_TRUE(is_position_free_horizontal_i(board, 0, 0, 0));

    board->big_piece = encode_big_square_position(0, 3);
    TEST_ASSERT_FALSE(is_position_free_horizontal_i(board, 0, 3, 0));

    destroy_board(board);
}

void test_is_position_free_horizontal_i_collision_with_other(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(0, 3);
    board->num_horizontal = 2;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 0);
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 1, 0, 2);

    TEST_ASSERT_FALSE(is_position_free_horizontal_i(board, 0, 2, 0));

    TEST_ASSERT_TRUE(is_position_free_horizontal_i(board, 0, 0, 0));

    TEST_ASSERT_FALSE(is_position_free_horizontal_i(board, 0, 0, 1));

    destroy_board(board);
}

void test_vertical_i_collides_with_horizontal_i(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(2, 3); // out of the way
    board->num_vertical = 1;
    board->num_horizontal = 1;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 1);

    // Vertical I at (0,0) occupies (0,0) and (0,1)
    // Horizontal I at (0,1) occupies (0,1) and (1,1)
    // They overlap at (0,1)

    // Vertical piece trying position (0,0): its top cell (0,0) is fine but
    // its bottom cell (0,1) collides with horizontal piece's (0,1)
    TEST_ASSERT_FALSE(is_position_free_vertical_i(board, 0, 0, 0));
    // Vertical piece at (2,0) occupies (2,0)+(2,1), no overlap
    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 2, 0, 0));

    // Horizontal piece trying position (0,1): its left cell (0,1) collides
    // with vertical piece's bottom cell (0,1)
    TEST_ASSERT_FALSE(is_position_free_horizontal_i(board, 0, 1, 0));
    // Horizontal piece at (0,3) occupies (0,3)+(1,3), no overlap
    TEST_ASSERT_TRUE(is_position_free_horizontal_i(board, 0, 3, 0));

    destroy_board(board);
}

void test_vertical_i_collides_with_small_block(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(0, 3); // out of the way
    board->num_small = 1;
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->num_vertical = 1;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 2, 0);

    // Small block at (2,2). Vertical I at (2,1) occupies (2,1) and (2,2) -> overlap at (2,2)
    TEST_ASSERT_FALSE(is_position_free_vertical_i(board, 2, 1, 0));

    // Vertical I at (0,0) occupies (0,0)+(0,1), no overlap with small block
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);
    TEST_ASSERT_TRUE(is_position_free_vertical_i(board, 0, 0, 0));

    destroy_board(board);
}

void test_horizontal_i_collides_with_small_block(void) {
    board_t *board = build_board();
    board->big_piece = encode_big_square_position(0, 3); // out of the way
    board->num_small = 1;
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 2, 2);
    board->num_horizontal = 1;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 1, 2);

    // Small block at (2,2). Horizontal I at (1,2) occupies (1,2) and (2,2) -> overlap at (2,2)
    TEST_ASSERT_FALSE(is_position_free_horizontal_i(board, 1, 2, 0));

    // Horizontal I at (0,0) occupies (0,0)+(1,0), no overlap
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 0, 0);
    TEST_ASSERT_TRUE(is_position_free_horizontal_i(board, 0, 0, 0));

    destroy_board(board);
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_build_and_destroy);
    RUN_TEST(test_is_position_free);
    RUN_TEST(test_is_position_free_big_square);

    RUN_TEST(test_get_big_piece_position);
    RUN_TEST(test_encode_big_piece_position);
    RUN_TEST(test_get_big_piece_position_idempotency);

    RUN_TEST(test_get_small_piece_position);
    RUN_TEST(test_set_small_piece_position);
    RUN_TEST(test_set_and_get_small_piece_idempotency);
    RUN_TEST(test_is_position_free_small_block);

    RUN_TEST(test_encode_vertical_i_position);
    RUN_TEST(test_get_vertical_i_position);
    RUN_TEST(test_set_vertical_i_position);
    RUN_TEST(test_set_and_get_vertical_i_idempotency);
    RUN_TEST(test_is_position_free_vertical_i);
    RUN_TEST(test_is_position_free_vertical_i_collision_with_other);

    RUN_TEST(test_encode_horizontal_i_position);
    RUN_TEST(test_get_horizontal_i_position);
    RUN_TEST(test_set_horizontal_i_position);
    RUN_TEST(test_set_and_get_horizontal_i_idempotency);
    RUN_TEST(test_is_position_free_horizontal_i);
    RUN_TEST(test_is_position_free_horizontal_i_collision_with_other);

    RUN_TEST(test_vertical_i_collides_with_horizontal_i);
    RUN_TEST(test_vertical_i_collides_with_small_block);
    RUN_TEST(test_horizontal_i_collides_with_small_block);

	return UNITY_END();
}
