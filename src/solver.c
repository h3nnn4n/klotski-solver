#include <assert.h>
#include <stdlib.h>

#include "board.h"
#include "solver.h"

solver_context_t *build_solver_context() {
    solver_context_t *solver_context = malloc(sizeof(solver_context_t));

    assert(solver_context != NULL);

    solver_context->board = build_board();
    solver_context->pdb   = NULL;

    return solver_context;
}

void destroy_solver_context(solver_context_t *solver_context) {
    assert(solver_context != NULL);
    assert(solver_context->board != NULL);

    destroy_board(solver_context->board);
    free(solver_context);
}
