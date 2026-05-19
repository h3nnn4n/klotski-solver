#include "pdb_2x2.h"

#include <stdlib.h>

#include "pdb.h"

void pdb_build_2x2_mdist(void) {
    pdb_t *old = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 12, 1);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            size_t  index = x + y * 3;
            uint8_t dist  = (uint8_t)(abs(x - 1) + abs(y - 3));
            pdb_set_entry(pdb, index, &dist);
        }
    }

    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, pdb);

    pdb_save_chunk(pdb, 0);

    if (old != NULL)
        pdb_destroy(old);
}
