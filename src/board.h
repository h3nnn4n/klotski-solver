#include <stdbool.h>

#include "stdint.h"

typedef struct _board {
    // we might need to track vertical and horizontal separatelly
    uint64_t i_blocks;

    uint64_t small_blocks;

    uint_fast8_t big_piece;
} board_t;

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

board_t* build_board();
void destroy_board(board_t *board);

bool is_position_free(board_t *board, uint_fast16_t x, uint_fast16_t y);
bool is_position_free_big_square(board_t *board, uint_fast16_t x, uint_fast16_t y);

uint_fast16_t get_x_position_from_big_square(uint_fast16_t piece);
uint_fast16_t get_y_position_from_big_square(uint_fast16_t piece);
