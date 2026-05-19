#ifndef SRC_PDB_H_
#define SRC_PDB_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PDB_CHUNK_SIZE (64 * 1024 * 1024)
#define PDB_CHUNK_PAD  4
#define PDB_CACHE_DIR  "cache/pdb"

typedef enum { PDB_BIG_SQUARE_MANHATTAN = 0, PDB_TYPE_COUNT } pdb_type_t;

typedef struct _solver_context solver_context_t;

typedef struct {
    pdb_type_t type;
    size_t     total_entries;
    size_t     entry_size;
    size_t     entries_per_chunk;
    size_t     total_chunks;
    void      *data;
    bool       loaded;
    size_t     loaded_chunks;
} pdb_t;

pdb_t *pdb_build(pdb_type_t type, size_t total_entries, size_t entry_size);
void   pdb_destroy(pdb_t *pdb);

void pdb_set_entry(pdb_t *pdb, size_t index, const void *value);
void pdb_get_entry(const pdb_t *pdb, size_t index, void *out_value);

pdb_t *pdb_get_global(pdb_type_t type);
void   pdb_set_global(pdb_type_t type, pdb_t *pdb);

const char *pdb_get_type_folder_name(pdb_type_t type);

size_t pdb_chunk_entry_count(const pdb_t *pdb, size_t chunk_index);

bool pdb_save_chunk(const pdb_t *pdb, size_t chunk_index);
bool pdb_load_chunk(pdb_t *pdb, size_t chunk_index);
bool pdb_load_from_disk(pdb_t *pdb);

size_t pdb_count_existing_chunks(const pdb_t *pdb);

void pdb_attach_to_solver(solver_context_t *ctx);

#endif
