#include "pdb.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static pdb_t *loaded_pdbs[PDB_TYPE_COUNT];

pdb_t *pdb_build(pdb_type_t type, size_t total_entries, size_t entry_size) {
    assert(entry_size > 0);

    pdb_t *pdb = malloc(sizeof(pdb_t));
    assert(pdb != NULL);

    size_t entries_per = PDB_CHUNK_SIZE / entry_size;
    if (entries_per == 0)
        entries_per = 1;

    size_t total_chunks = (total_entries + entries_per - 1) / entries_per;

    pdb->type              = type;
    pdb->total_entries     = total_entries;
    pdb->entry_size        = entry_size;
    pdb->entries_per_chunk = entries_per;
    pdb->total_chunks      = total_chunks;
    pdb->data              = calloc(total_entries, entry_size);
    assert(pdb->data != NULL);
    pdb->loaded        = false;
    pdb->loaded_chunks = 0;

    return pdb;
}

void pdb_destroy(pdb_t *pdb) {
    assert(pdb != NULL);
    free(pdb->data);
    free(pdb);
}

void pdb_set_entry(pdb_t *pdb, size_t index, const void *value) {
    assert(pdb != NULL);
    assert(index < pdb->total_entries);
    memcpy((char *)pdb->data + index * pdb->entry_size, value, pdb->entry_size);
}

void pdb_get_entry(const pdb_t *pdb, size_t index, void *out_value) {
    assert(pdb != NULL);
    assert(index < pdb->total_entries);
    memcpy(out_value, (const char *)pdb->data + index * pdb->entry_size, pdb->entry_size);
}

pdb_t *pdb_get_global(pdb_type_t type) {
    assert(type < PDB_TYPE_COUNT);
    return loaded_pdbs[type];
}

void pdb_set_global(pdb_type_t type, pdb_t *pdb) {
    assert(type < PDB_TYPE_COUNT);
    loaded_pdbs[type] = pdb;
}
