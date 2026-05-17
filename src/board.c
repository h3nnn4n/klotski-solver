#include "board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

board_t *build_board() {
    board_t *board = (board_t *)malloc(sizeof(board_t));
    assert(board != NULL);

    board->big_piece = 0;

    board->vertical_blocks = 0;
    board->num_vertical    = 0;

    board->horizontal_blocks = 0;
    board->num_horizontal    = 0;

    board->small_blocks = 0;

    return board;
}

void destroy_board(board_t *board) {
    assert(board != NULL);

    free(board);
}

void reset_board_to_classic(board_t *board) {
    board->small_blocks = set_small_block_position(board->small_blocks, 0, 0, 4);
    board->small_blocks = set_small_block_position(board->small_blocks, 1, 1, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 2, 2, 3);
    board->small_blocks = set_small_block_position(board->small_blocks, 3, 3, 4);

    board->big_piece = set_big_square_position(1, 0);

    board->num_vertical    = 4;
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 0, 0, 0);
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 1, 0, 2);
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 2, 3, 0);
    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, 3, 3, 2);

    board->num_horizontal    = 1;
    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, 0, 1, 2);
}

bool is_board_valid(board_t *board) {
    if (!is_small_block_position_valid(board->small_blocks))
        return false;
    if (!is_horizontal_i_position_valid(board->horizontal_blocks, board->num_horizontal))
        return false;
    if (!is_vertical_i_position_valid(board->vertical_blocks, board->num_vertical))
        return false;

    if (board->num_horizontal + board->num_vertical != 5)
        return false;

    uint_fast16_t num_empty_cells = 0;

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 5; y++) {
            if (is_position_free(board, x, y)) {
                num_empty_cells++;
            }
        }
    }

    if (num_empty_cells != 2)
        return false;

    return true;
}

bool is_position_free(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    if (!is_position_free_of_big_square(board, x, y))
        return false;

    if (!is_position_free_of_small_block(board, x, y))
        return false;

    if (!is_position_free_of_vertical_i(board, x, y))
        return false;

    if (!is_position_free_of_horizontal_i(board, x, y))
        return false;

    return true;
}

// =============================================================================
// 2x2 functions
// =============================================================================

bool is_position_free_of_big_square(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    uint_fast16_t piece_x = get_x_position_from_big_square(board->big_piece);
    uint_fast16_t piece_y = get_y_position_from_big_square(board->big_piece);

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

bool does_big_square_fit_in(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 2);
    assert(y <= 3);

    return is_position_free(board, x, y) && is_position_free(board, x + 1, y) && is_position_free(board, x, y + 1) &&
           is_position_free(board, x + 1, y + 1);
}

uint_fast16_t get_x_position_from_big_square(uint_fast16_t piece) { return piece % 3; }

uint_fast16_t get_y_position_from_big_square(uint_fast16_t piece) { return piece / 3; }

uint_fast16_t set_big_square_position(uint_fast16_t x, uint_fast16_t y) { return x + y * 3; }

// =============================================================================
// 1x1 functions
// =============================================================================

bool is_position_free_of_small_block(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    for (uint_fast8_t i = 0; i < 4; i++) {
        uint64_t      piece_pos = (board->small_blocks >> (i * 5)) & 0x1F;
        uint_fast16_t piece_x   = piece_pos % 4;
        uint_fast16_t piece_y   = piece_pos / 4;
        if (x == piece_x && y == piece_y)
            return false;
    }

    return true;
}

bool does_small_block_fit_in(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    return is_position_free(board, x, y);
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

bool is_small_block_position_valid(uint64_t pieces) {
    uint64_t used_x[4];
    uint64_t used_y[4];

    for (int index = 0; index < 4; index++) {
        used_x[index] = get_x_position_from_small_block(pieces, index);
        used_y[index] = get_y_position_from_small_block(pieces, index);

        for (int check_index = 0; check_index < index; check_index++) {
            if (used_x[index] == used_x[check_index] && used_y[index] == used_y[check_index]) {
                return false;
            }
        }
    }

    return true;
}

// =============================================================================
// Vertical I-piece (2x1) functions
// =============================================================================

uint_fast16_t get_x_position_from_vertical_i(uint64_t pieces, uint_fast8_t index) {
    assert(index < 10);
    uint64_t pos = (pieces >> (index * 4)) & 0xF;
    return pos % 4;
}

uint_fast16_t get_y_position_from_vertical_i(uint64_t pieces, uint_fast8_t index) {
    assert(index < 10);
    uint64_t pos = (pieces >> (index * 4)) & 0xF;
    return pos / 4;
}

uint64_t set_vertical_i_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y) {
    assert(index < 10);
    assert(x <= 3);
    assert(y <= 3);

    uint64_t encoded = x + y * 4;
    uint64_t mask    = ~((uint64_t)0xF << (index * 4));

    pieces &= mask;
    pieces |= encoded << (index * 4);

    return pieces;
}

bool is_vertical_i_position_valid(uint64_t pieces, uint_fast8_t num_pieces) {
    uint64_t used_x[10];
    uint64_t used_y[10];

    for (int index = 0; index < num_pieces; index++) {
        used_x[index] = get_x_position_from_vertical_i(pieces, index);
        used_y[index] = get_y_position_from_vertical_i(pieces, index);

        for (int check_index = 0; check_index < index; check_index++) {
            if (used_x[index] == used_x[check_index] && used_y[index] <= used_y[check_index] + 1 &&
                used_y[index] + 1 >= used_y[check_index]) {
                return false;
            }
        }
    }

    return true;
}

bool is_position_free_of_vertical_i(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        if (x == vx && (y == vy || y == vy + 1))
            return false;
    }

    return true;
}

bool does_vertical_i_fit_in(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 3);

    return is_position_free(board, x, y) && is_position_free(board, x, y + 1);
}

// =============================================================================
// Horizontal I-piece (1x2) functions
// =============================================================================

uint_fast16_t get_x_position_from_horizontal_i(uint64_t pieces, uint_fast8_t index) {
    assert(index < 10);
    uint64_t pos = (pieces >> (index * 4)) & 0xF;
    return pos % 3;
}

uint_fast16_t get_y_position_from_horizontal_i(uint64_t pieces, uint_fast8_t index) {
    assert(index < 10);
    uint64_t pos = (pieces >> (index * 4)) & 0xF;
    return pos / 3;
}

uint64_t set_horizontal_i_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y) {
    assert(index < 10);
    assert(x <= 2);
    assert(y <= 4);

    uint64_t encoded = x + y * 3;
    uint64_t mask    = ~((uint64_t)0xF << (index * 4));

    pieces &= mask;
    pieces |= encoded << (index * 4);

    return pieces;
}

bool is_horizontal_i_position_valid(uint64_t pieces, uint_fast8_t num_pieces) {
    uint64_t used_x[10];
    uint64_t used_y[10];

    for (int index = 0; index < num_pieces; index++) {
        used_x[index] = get_x_position_from_horizontal_i(pieces, index);
        used_y[index] = get_y_position_from_horizontal_i(pieces, index);

        for (int check_index = 0; check_index < index; check_index++) {
            if (used_y[index] == used_y[check_index] && used_x[index] <= used_x[check_index] + 1 &&
                used_x[index] + 1 >= used_x[check_index]) {
                return false;
            }
        }
    }

    return true;
}

bool is_position_free_of_horizontal_i(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 4);

    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        if (y == hy && (x == hx || x == hx + 1))
            return false;
    }

    return true;
}

bool does_horizontal_i_fit_in(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 2);
    assert(y <= 4);

    return is_position_free(board, x, y) && is_position_free(board, x + 1, y);
}
