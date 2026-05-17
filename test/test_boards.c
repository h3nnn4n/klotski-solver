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

	return UNITY_END();
}
