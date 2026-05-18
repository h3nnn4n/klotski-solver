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

bool is_board_valid(const board_t *board) {
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

bool is_board_solved(const board_t *board) {
    uint_fast16_t bx = get_x_position_from_big_square(board->big_piece);
    uint_fast16_t by = get_y_position_from_big_square(board->big_piece);
    return bx == 1 && by == 3;
}

bool is_position_free(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

piece_type_t get_piece_at(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
    {
        uint_fast16_t bx = get_x_position_from_big_square(board->big_piece);
        uint_fast16_t by = get_y_position_from_big_square(board->big_piece);
        if (x >= bx && x <= bx + 1 && y >= by && y <= by + 1)
            return PIECE_BIG_SQUARE;
    }

    for (uint_fast8_t i = 0; i < 4; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        if (x == sx && y == sy)
            return PIECE_SMALL_BLOCK;
    }

    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        if (x == vx && (y == vy || y == vy + 1))
            return PIECE_VERTICAL_I;
    }

    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        if (y == hy && (x == hx || x == hx + 1))
            return PIECE_HORIZONTAL_I;
    }

    return PIECE_NONE;
}

does_piece_fit_fn get_does_piece_fit_fn(piece_type_t type) {
    switch (type) {
        case PIECE_BIG_SQUARE: return does_big_square_fit_in;
        case PIECE_SMALL_BLOCK: return does_small_block_fit_in;
        case PIECE_VERTICAL_I: return does_vertical_i_fit_in;
        case PIECE_HORIZONTAL_I: return does_horizontal_i_fit_in;
        default: return NULL;
    }
}

board_t board_clone(const board_t *board) {
    assert(board != NULL);
    return *board;
}

bool is_board_equal(const board_t *a, const board_t *b) {
    return a->big_piece == b->big_piece && a->small_blocks == b->small_blocks && a->num_vertical == b->num_vertical &&
           a->vertical_blocks == b->vertical_blocks && a->num_horizontal == b->num_horizontal &&
           a->horizontal_blocks == b->horizontal_blocks;
}

void board_remove_big_square(board_t *board) {
    assert(board != NULL);
    board->big_piece = 15;
}

void board_remove_small_block(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(board != NULL);
    assert(x <= 3);
    assert(y <= 4);
    for (uint_fast8_t i = 0; i < 4; i++) {
        uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
        uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
        if (sx == x && sy == y) {
            board->small_blocks |= ((uint64_t)0x1F << (i * 5));
            return;
        }
    }
    assert(!"no small block at given cell");
}

void board_remove_vertical_i(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(board != NULL);
    assert(x <= 3);
    assert(y <= 4);
    assert(board->num_vertical > 0);
    for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
        uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
        uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
        if (vx == x && (y == vy || y == vy + 1)) {
            if (i < board->num_vertical - 1) {
                board->vertical_blocks = set_vertical_i_position(
                    board->vertical_blocks, i,
                    get_x_position_from_vertical_i(board->vertical_blocks, board->num_vertical - 1),
                    get_y_position_from_vertical_i(board->vertical_blocks, board->num_vertical - 1));
            }
            board->num_vertical--;
            return;
        }
    }
    assert(!"no vertical I at given cell");
}

void board_remove_horizontal_i(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(board != NULL);
    assert(x <= 3);
    assert(y <= 4);
    assert(board->num_horizontal > 0);
    for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
        uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
        uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
        if (hy == y && (x == hx || x == hx + 1)) {
            if (i < board->num_horizontal - 1) {
                board->horizontal_blocks = set_horizontal_i_position(
                    board->horizontal_blocks, i,
                    get_x_position_from_horizontal_i(board->horizontal_blocks, board->num_horizontal - 1),
                    get_y_position_from_horizontal_i(board->horizontal_blocks, board->num_horizontal - 1));
            }
            board->num_horizontal--;
            return;
        }
    }
    assert(!"no horizontal I at given cell");
}

void board_remove_piece(board_t *board, piece_type_t type, uint_fast16_t x, uint_fast16_t y) {
    assert(board != NULL);
    switch (type) {
        case PIECE_BIG_SQUARE: board_remove_big_square(board); break;
        case PIECE_SMALL_BLOCK: board_remove_small_block(board, x, y); break;
        case PIECE_VERTICAL_I: board_remove_vertical_i(board, x, y); break;
        case PIECE_HORIZONTAL_I: board_remove_horizontal_i(board, x, y); break;
        default: assert(!"board_remove_piece: invalid piece type"); break;
    }
}

void board_get_piece_position(const board_t *board, piece_type_t type, uint_fast16_t x, uint_fast16_t y,
                              uint_fast16_t *out_px, uint_fast16_t *out_py) {
    assert(board != NULL);

    switch (type) {
        case PIECE_BIG_SQUARE:
            *out_px = get_x_position_from_big_square(board->big_piece);
            *out_py = get_y_position_from_big_square(board->big_piece);
            return;
        case PIECE_SMALL_BLOCK:
            for (uint_fast8_t i = 0; i < 4; i++) {
                uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
                uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
                if (sx == x && sy == y) {
                    *out_px = sx;
                    *out_py = sy;
                    return;
                }
            }
            break;
        case PIECE_VERTICAL_I:
            for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
                uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
                uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
                if (vx == x && (y == vy || y == vy + 1)) {
                    *out_px = vx;
                    *out_py = vy;
                    return;
                }
            }
            break;
        case PIECE_HORIZONTAL_I:
            for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
                uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
                uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
                if (hy == y && (x == hx || x == hx + 1)) {
                    *out_px = hx;
                    *out_py = hy;
                    return;
                }
            }
            break;
        default: break;
    }
    assert(!"board_get_piece_position: no piece found at cell");
}

void board_move_piece_to(board_t *board, piece_type_t type, uint_fast16_t cell_x, uint_fast16_t cell_y,
                         uint_fast16_t new_x, uint_fast16_t new_y) {
    assert(board != NULL);

    switch (type) {
        case PIECE_BIG_SQUARE: board->big_piece = set_big_square_position(new_x, new_y); return;
        case PIECE_SMALL_BLOCK:
            for (uint_fast8_t i = 0; i < 4; i++) {
                uint_fast16_t sx = get_x_position_from_small_block(board->small_blocks, i);
                uint_fast16_t sy = get_y_position_from_small_block(board->small_blocks, i);
                if (sx == cell_x && sy == cell_y) {
                    board->small_blocks = set_small_block_position(board->small_blocks, i, new_x, new_y);
                    return;
                }
            }
            break;
        case PIECE_VERTICAL_I:
            for (uint_fast8_t i = 0; i < board->num_vertical; i++) {
                uint_fast16_t vx = get_x_position_from_vertical_i(board->vertical_blocks, i);
                uint_fast16_t vy = get_y_position_from_vertical_i(board->vertical_blocks, i);
                if (vx == cell_x && (cell_y == vy || cell_y == vy + 1)) {
                    board->vertical_blocks = set_vertical_i_position(board->vertical_blocks, i, new_x, new_y);
                    return;
                }
            }
            break;
        case PIECE_HORIZONTAL_I:
            for (uint_fast8_t i = 0; i < board->num_horizontal; i++) {
                uint_fast16_t hx = get_x_position_from_horizontal_i(board->horizontal_blocks, i);
                uint_fast16_t hy = get_y_position_from_horizontal_i(board->horizontal_blocks, i);
                if (hy == cell_y && (cell_x == hx || cell_x == hx + 1)) {
                    board->horizontal_blocks = set_horizontal_i_position(board->horizontal_blocks, i, new_x, new_y);
                    return;
                }
            }
            break;
        default: break;
    }
    assert(!"board_move_piece_to: no piece found at cell");
}

void board_move_piece_to_empty_cell(board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(can_piece_move(board, x, y));

    piece_type_t piece_type = get_piece_at(board, x, y);
    assert(piece_type != PIECE_NONE);

    uint_fast16_t piece_x, piece_y;
    board_get_piece_position(board, piece_type, x, y, &piece_x, &piece_y);

    int max_x, max_y;
    switch (piece_type) {
        case PIECE_BIG_SQUARE:
            max_x = 2;
            max_y = 3;
            break;
        case PIECE_SMALL_BLOCK:
            max_x = 3;
            max_y = 4;
            break;
        case PIECE_VERTICAL_I:
            max_x = 3;
            max_y = 3;
            break;
        case PIECE_HORIZONTAL_I:
            max_x = 2;
            max_y = 4;
            break;
        default:
            max_x = 3;
            max_y = 4;
            break;
    }

    does_piece_fit_fn fn = get_does_piece_fit_fn(piece_type);
    assert(fn != NULL);

    board_t copy = board_clone(board);
    board_remove_piece(&copy, piece_type, x, y);

    int           offsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    int           found         = 0;
    uint_fast16_t new_x = 0, new_y = 0;

    for (int d = 0; d < 4; d++) {
        int nx = (int)x + offsets[d][0];
        int ny = (int)y + offsets[d][1];
        if (nx < 0 || ny < 0 || nx > max_x || ny > max_y)
            continue;
        if ((uint_fast16_t)nx == piece_x && (uint_fast16_t)ny == piece_y)
            continue;
        if (fn(&copy, (uint_fast16_t)nx, (uint_fast16_t)ny)) {
            new_x = (uint_fast16_t)nx;
            new_y = (uint_fast16_t)ny;
            found++;
        }
    }

    assert(found == 1);
    board_move_piece_to(board, piece_type, x, y, new_x, new_y);
}

bool can_piece_move(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
    piece_type_t piece_type = get_piece_at(board, x, y);
    if (piece_type == PIECE_NONE)
        return false;

    uint_fast16_t piece_x, piece_y;
    board_get_piece_position(board, piece_type, x, y, &piece_x, &piece_y);

    does_piece_fit_fn fn = get_does_piece_fit_fn(piece_type);
    assert(fn != NULL);

    board_t copy = board_clone(board);
    board_remove_piece(&copy, piece_type, x, y);

    int max_x, max_y;
    switch (piece_type) {
        case PIECE_BIG_SQUARE:
            max_x = 2;
            max_y = 3;
            break;
        case PIECE_SMALL_BLOCK:
            max_x = 3;
            max_y = 4;
            break;
        case PIECE_VERTICAL_I:
            max_x = 3;
            max_y = 3;
            break;
        case PIECE_HORIZONTAL_I:
            max_x = 2;
            max_y = 4;
            break;
        default: return false;
    }

    int offsets[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int d = 0; d < 4; d++) {
        int nx = (int)x + offsets[d][0];
        int ny = (int)y + offsets[d][1];
        if (nx < 0 || ny < 0 || nx > max_x || ny > max_y)
            continue;
        if ((uint_fast16_t)nx == piece_x && (uint_fast16_t)ny == piece_y)
            continue;
        if (fn(&copy, (uint_fast16_t)nx, (uint_fast16_t)ny))
            return true;
    }

    return false;
}

// =============================================================================
// 2x2 functions
// =============================================================================

bool is_position_free_of_big_square(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool does_big_square_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool is_position_free_of_small_block(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool does_small_block_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool is_position_free_of_vertical_i(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool does_vertical_i_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool is_position_free_of_horizontal_i(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
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

bool does_horizontal_i_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y) {
    assert(x <= 2);
    assert(y <= 4);

    return is_position_free(board, x, y) && is_position_free(board, x + 1, y);
}
