#include "board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

board_t *build_board() {
    board_t *board = (board_t *)malloc(sizeof(board_t));
    assert(board != NULL);

    board->big_piece    = 0;
    board->i_blocks     = 0;
    board->small_blocks = 0;

    return board;
}

void destroy_board(board_t *board) {
    assert(board != NULL);

    free(board);
}

bool is_position_free(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    board->i_blocks++;
    assert(x <= 4);
    assert(y <= 5);

    return true;
}

bool is_position_free_big_square(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    uint_fast16_t piece_x = get_x_position_from_big_square(board->big_piece);
    uint_fast16_t piece_y = get_x_position_from_big_square(board->big_piece);

    if (x == piece_x && y == piece_y)
        return false;
    if (x == piece_x + 1 && y == piece_y)
        return false;
    if (x == piece_x && y == piece_y + 1)
        return false;
    if (x == piece_x + 1 && y == piece_y + 1)
        return false;

    return true;
}

uint_fast16_t get_x_position_from_big_square(uint_fast16_t piece) { return piece % 3; }

uint_fast16_t get_y_position_from_big_square(uint_fast16_t piece) { return piece / 3; }
