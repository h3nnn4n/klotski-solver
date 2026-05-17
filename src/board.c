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
    board->num_small    = 0;

    return board;
}

void destroy_board(board_t *board) {
    assert(board != NULL);

    free(board);
}

bool is_position_free(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    (void)board;
    assert(x <= 4);
    assert(y <= 5);

    return true;
}

// =============================================================================
// 2x2 functions
// =============================================================================

bool is_position_free_big_square(board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

uint_fast16_t get_x_position_from_big_square(uint_fast16_t piece) { return piece % 3; }

uint_fast16_t get_y_position_from_big_square(uint_fast16_t piece) { return piece / 3; }

uint_fast16_t encode_big_square_position(uint_fast16_t x, uint_fast16_t y) { return x + y * 3; }

// =============================================================================
// 1x1 functions
// =============================================================================

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

// =============================================================================
// Vertical I-piece (2x1) functions
// =============================================================================

uint64_t encode_vertical_i_position(uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 3);
    assert(y <= 3);
    return x + y * 4;
}

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

bool is_position_free_vertical_i(board_t *board, uint_fast16_t x, uint_fast16_t y, uint_fast8_t index) {
    assert(x <= 3);
    assert(y <= 3);
    assert(index < board->num_vertical);

    // A vertical I at (x,y) occupies cells (x,y) [top] and (x,y+1) [bottom]

    // Check collision with big square
    if (!is_position_free_big_square(board, x, y))
        return false;
    if (!is_position_free_big_square(board, x, y + 1))
        return false;

    // Check collision with small blocks
    for (uint_fast8_t i = 0; i < board->num_small; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        // Small block occupies (sx, sy)
        if (x == sx && (y == sy || y + 1 == sy))
            return false;
    }

    // Check collision with other vertical I pieces (except self)
    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        if (i == index)
            continue;
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        // Other vertical I occupies (vx, vy) and (vx, vy+1)
        // Our cells: (x,y) and (x,y+1)
        if (x == vx && (y == vy || y == vy + 1 || y + 1 == vy || y + 1 == vy + 1))
            return false;
    }

    // Check collision with horizontal I pieces
    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        // Horizontal I occupies (hx, hy) and (hx+1, hy)
        // Our cells: (x,y) and (x,y+1)
        if (hy == y && (x == hx || x == hx + 1))
            return false;
        if (hy == y + 1 && (x == hx || x == hx + 1))
            return false;
    }

    return true;
}

// =============================================================================
// Horizontal I-piece (1x2) functions
// =============================================================================

uint64_t encode_horizontal_i_position(uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 2);
    assert(y <= 4);
    return x + y * 3;
}

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

bool is_position_free_horizontal_i(board_t *board, uint_fast16_t x, uint_fast16_t y, uint_fast8_t index) {
    assert(x <= 2);
    assert(y <= 4);
    assert(index < board->num_horizontal);

    // A horizontal I at (x,y) occupies cells (x,y) [left] and (x+1,y) [right]

    // Check collision with big square
    if (!is_position_free_big_square(board, x, y))
        return false;
    if (!is_position_free_big_square(board, x + 1, y))
        return false;

    // Check collision with small blocks
    for (uint_fast8_t i = 0; i < board->num_small; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        // Small block occupies (sx, sy)
        if (y == sy && (x == sx || x + 1 == sx))
            return false;
    }

    // Check collision with vertical I pieces
    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        // Vertical I occupies (vx, vy) and (vx, vy+1)
        // Our cells: (x,y) and (x+1,y)
        if (vx == x && (y == vy || y == vy + 1))
            return false;
        if (vx == x + 1 && (y == vy || y == vy + 1))
            return false;
    }

    // Check collision with other horizontal I pieces (except self)
    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        if (i == index)
            continue;
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        // Other horizontal I occupies (hx, hy) and (hx+1, hy)
        // Our cells: (x,y) and (x+1,y)
        if (y == hy && (x == hx || x == hx + 1 || x + 1 == hx || x + 1 == hx + 1))
            return false;
    }

    return true;
}
