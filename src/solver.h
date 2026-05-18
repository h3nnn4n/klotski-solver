#ifndef SRC_SOLVER_H_
#define SRC_SOLVER_H_

#include "board.h"

typedef struct _solver_context {
    board_t *board;
} solver_context_t;

solver_context_t *build_solver_context();
void              destroy_solver_context(solver_context_t *solver_context);

#endif
