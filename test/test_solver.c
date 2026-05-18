#include <stdint.h>
#include <unity.h>
#include "board.h"
#include "solver.h"

void setUp(void) {}
void tearDown(void) {}

void test_build_and_destroy_solver(void) {
    solver_context_t *ctx = build_solver_context();

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->board);

    destroy_solver_context(ctx);
}

void test_solver_board_starts_zeroed(void) {
    solver_context_t *ctx = build_solver_context();

    TEST_ASSERT_EQUAL(0, ctx->board->num_vertical);
    TEST_ASSERT_EQUAL(0, ctx->board->num_horizontal);
    TEST_ASSERT_EQUAL(0, ctx->board->small_blocks);
    TEST_ASSERT_EQUAL(0, ctx->board->big_piece);

    destroy_solver_context(ctx);
}

void test_solver_classic_board_is_valid(void) {
    solver_context_t *ctx = build_solver_context();
    reset_board_to_classic(ctx->board);

    TEST_ASSERT_TRUE(is_board_valid(ctx->board));

    destroy_solver_context(ctx);
}

void test_solver_classic_board_not_solved(void) {
    solver_context_t *ctx = build_solver_context();
    reset_board_to_classic(ctx->board);

    TEST_ASSERT_FALSE(is_board_solved(ctx->board));

    destroy_solver_context(ctx);
}

void test_solver_board_solved_when_big_square_at_bottom(void) {
    solver_context_t *ctx = build_solver_context();
    ctx->board->big_piece = set_big_square_position(1, 3);

    TEST_ASSERT_TRUE(is_board_solved(ctx->board));

    destroy_solver_context(ctx);
}

void test_solver_board_wrap_reset_classic(void) {
    solver_context_t *ctx = build_solver_context();

    reset_board_to_classic(ctx->board);

    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(ctx->board, 1, 0));
    TEST_ASSERT_EQUAL(PIECE_BIG_SQUARE, get_piece_at(ctx->board, 2, 1));
    TEST_ASSERT_EQUAL(PIECE_VERTICAL_I, get_piece_at(ctx->board, 0, 0));
    TEST_ASSERT_EQUAL(PIECE_VERTICAL_I, get_piece_at(ctx->board, 0, 1));
    TEST_ASSERT_EQUAL(PIECE_HORIZONTAL_I, get_piece_at(ctx->board, 1, 2));
    TEST_ASSERT_EQUAL(PIECE_SMALL_BLOCK, get_piece_at(ctx->board, 0, 4));
    TEST_ASSERT_EQUAL(PIECE_NONE, get_piece_at(ctx->board, 1, 4));

    destroy_solver_context(ctx);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_build_and_destroy_solver);
    RUN_TEST(test_solver_board_starts_zeroed);
    RUN_TEST(test_solver_classic_board_is_valid);
    RUN_TEST(test_solver_classic_board_not_solved);
    RUN_TEST(test_solver_board_solved_when_big_square_at_bottom);
    RUN_TEST(test_solver_board_wrap_reset_classic);

    return UNITY_END();
}
