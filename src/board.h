#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <stdbool.h>

#include <stdint.h>

typedef struct _board {
    // Vertical I pieces (2x1). Up to 5 pieces, each encoded as x + y*4 (4 bits each).
    uint64_t     vertical_blocks;
    uint_fast8_t num_vertical;

    // Horizontal I pieces (1x2). Up to 5 pieces, each encoded as x + y*3 (4 bits each).
    uint64_t     horizontal_blocks;
    uint_fast8_t num_horizontal;

    // Small 1x1 blocks (up to 4 pieces, each encoded as x + y*4, 5 bits each)
    uint64_t small_blocks;

    // Big 2x2 piece, encoded as x + y*3 (0-11)
    uint_fast8_t big_piece;
} board_t;

typedef enum {
    PIECE_NONE         = -1,
    PIECE_BIG_SQUARE   = 0,
    PIECE_SMALL_BLOCK  = 1,
    PIECE_VERTICAL_I   = 2,
    PIECE_HORIZONTAL_I = 3,
} piece_type_t;

typedef bool (*does_piece_fit_fn)(const board_t *, uint_fast16_t, uint_fast16_t);

/*
 *
 * Goal position:
 *  . . . .
 *  . . . .
 *  . . . .
 *  . 4 4 .
 *  . 4 4 .
 *
 * Board is 4x5
 * There are 4 pieces of size 1
 * There are 5 pieces of size 2
 * There is one piece 2x2
 *
 * One piece an occupy 20 positions
 * Two pieces can occupy 20 * 19 = 380
 * Three can occupy 20 * 19 * 18 = 6840
 * Four pieces can occupy 20 * 19 * 18 * 17 = 116280
 *
 * The big piece can occupy 12 positions
 *   3 x 4
 *   encoded position = x + y * 3
 *   0 0 = 0
 *   1 0 = 1
 *   2 0 = 2
 *   0 1 = 3
 *
 *
 *
 *
 *
 */

board_t *build_board();
void     destroy_board(board_t *board);

// Board
bool              is_position_free(const board_t *board, uint_fast16_t x, uint_fast16_t y);
piece_type_t      get_piece_at(const board_t *board, uint_fast16_t x, uint_fast16_t y);
void              reset_board_to_classic(board_t *board);
bool              is_board_valid(const board_t *board);
bool              is_board_solved(const board_t *board);
bool              can_piece_move(const board_t *board, uint_fast16_t x, uint_fast16_t y);
does_piece_fit_fn get_does_piece_fit_fn(piece_type_t type);

board_t board_clone(const board_t *board);
void    board_remove_big_square(board_t *board);
void    board_remove_small_block(board_t *board, uint_fast16_t x, uint_fast16_t y);
void    board_remove_vertical_i(board_t *board, uint_fast16_t x, uint_fast16_t y);
void    board_remove_horizontal_i(board_t *board, uint_fast16_t x, uint_fast16_t y);
void    board_remove_piece(board_t *board, piece_type_t type, uint_fast16_t x, uint_fast16_t y);

void board_get_piece_position(const board_t *board, piece_type_t type, uint_fast16_t x, uint_fast16_t y,
                              uint_fast16_t *out_px, uint_fast16_t *out_py);
void board_move_piece_to(board_t *board, piece_type_t type, uint_fast16_t cell_x, uint_fast16_t cell_y,
                         uint_fast16_t new_x, uint_fast16_t new_y);
void board_move_piece_to_empty_cell(board_t *board, uint_fast16_t x, uint_fast16_t y);

// 2x2 piece functions
bool          is_position_free_of_big_square(const board_t *board, uint_fast16_t x, uint_fast16_t y);
bool          does_big_square_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y);
uint_fast16_t get_x_position_from_big_square(uint_fast16_t piece);
uint_fast16_t get_y_position_from_big_square(uint_fast16_t piece);
uint_fast16_t set_big_square_position(uint_fast16_t x, uint_fast16_t y);

// 1x1 piece functions
bool          is_position_free_of_small_block(const board_t *board, uint_fast16_t x, uint_fast16_t y);
bool          does_small_block_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y);
uint_fast16_t get_x_position_from_small_block(uint64_t pieces, uint_fast8_t index);
uint_fast16_t get_y_position_from_small_block(uint64_t pieces, uint_fast8_t index);
uint64_t      set_small_block_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y);
bool          is_small_block_position_valid(uint64_t pieces);

// Vertical I-piece (2x1) functions
bool          is_position_free_of_vertical_i(const board_t *board, uint_fast16_t x, uint_fast16_t y);
bool          does_vertical_i_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y);
uint_fast16_t get_x_position_from_vertical_i(uint64_t pieces, uint_fast8_t index);
uint_fast16_t get_y_position_from_vertical_i(uint64_t pieces, uint_fast8_t index);
uint64_t      set_vertical_i_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y);
bool          is_vertical_i_position_valid(uint64_t pieces, uint_fast8_t num_pieces);

// Horizontal I-piece (1x2) functions
bool          is_position_free_of_horizontal_i(const board_t *board, uint_fast16_t x, uint_fast16_t y);
bool          does_horizontal_i_fit_in(const board_t *board, uint_fast16_t x, uint_fast16_t y);
uint_fast16_t get_x_position_from_horizontal_i(uint64_t pieces, uint_fast8_t index);
uint_fast16_t get_y_position_from_horizontal_i(uint64_t pieces, uint_fast8_t index);
uint64_t      set_horizontal_i_position(uint64_t pieces, uint_fast8_t index, uint_fast16_t x, uint_fast16_t y);
bool          is_horizontal_i_position_valid(uint64_t pieces, uint_fast8_t num_pieces);

#endif
