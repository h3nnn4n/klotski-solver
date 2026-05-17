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

uint_fast16_t encode_big_square_position(uint_fast16_t x, uint_fast16_t y) { return x + y * 3; }

bool is_position_free_small_block(board_t *board, uint_fast16_t x, uint_fast16_t y, uint_fast8_t index) {
    assert(x <= 3);
    assert(y <= 4);
    assert(index < 4);

    if (!is_position_free_big_square(board, x, y))
        return false;

    uint64_t      piece_pos = (board->small_blocks >> (index * 5)) & 0x1F;
    uint_fast16_t piece_x   = piece_pos % 4;
    uint_fast16_t piece_y   = piece_pos / 4;

    return !(x == piece_x && y == piece_y);
}

uint_fast16_t get_x_position_from_small_block(uint64_t pieces, uint_fast8_t index) {
    assert(index < 4);

    uint64_t pos = (pieces >> (index * 5)) & 0x1F;
    return pos % 4;
}

uint_fast16_t get_y_position_from_small_block(uint64_t pieces, uint_fast8_t index) {
    assert(index < 4);

    uint64_t pos = (pieces >> (index * 5)) & 0x1F;
    return pos / 4;
}

uint64_t set_small_block_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y) {
    assert(index < 4);
    assert(x <= 3);
    assert(y <= 4);

    uint64_t encoded = x + y * 4;
    uint64_t mask    = ~((uint64_t)0x1F << (index * 5));

    pieces &= mask;
    pieces |= encoded << (index * 5);

    return pieces;
}
