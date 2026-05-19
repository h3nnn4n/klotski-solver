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

typedef void (*pdb_progress_fn)(pdb_type_t type, size_t current, size_t total, void *user_data);

typedef struct {
    pdb_type_t  type;
    const char *name;
    const char *pieces;
    size_t      total_entries;
    size_t      entry_size;
} pdb_type_info_t;

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

bool pdb_validate_chunk(const pdb_t *pdb, size_t chunk_index);
bool pdb_validate_all(const pdb_t *pdb);

void pdb_attach_to_solver(solver_context_t *ctx);

const pdb_type_info_t *pdb_get_type_info(pdb_type_t type);
pdb_type_t             pdb_find_type_by_name(const char *name);
size_t                 pdb_get_total_size_on_disk(pdb_type_t type);

void pdb_build_all(pdb_progress_fn progress, void *user_data);
void pdb_rebuild_all(pdb_progress_fn progress, void *user_data);
void pdb_delete_cache_all(void);
void pdb_verify_all(void);
void pdb_list_all(void);

void pdb_build_named(const char *name, pdb_progress_fn progress, void *user_data);
void pdb_rebuild_named(const char *name, pdb_progress_fn progress, void *user_data);
void pdb_delete_cache_named(const char *name);
void pdb_verify_named(const char *name);

#endif
