#include "pdb.h"
#include "pdb_2x2.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "solver.h"

static pdb_t *loaded_pdbs[PDB_TYPE_COUNT];

static const char *folder_names[] = {
    "pdb_big_square_manhattan",
};

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

const char *pdb_get_type_folder_name(pdb_type_t type) {
    assert(type < PDB_TYPE_COUNT);
    return folder_names[type];
}

size_t pdb_chunk_entry_count(const pdb_t *pdb, size_t chunk_index) {
    assert(pdb != NULL);
    assert(chunk_index < pdb->total_chunks);

    if (chunk_index < pdb->total_chunks - 1)
        return pdb->entries_per_chunk;
    return pdb->total_entries - chunk_index * pdb->entries_per_chunk;
}

static void mkdir_parents(const char *path) {
    char  tmp[1024];
    char *end = tmp + sizeof(tmp) - 1;

    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (p < end)
                mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

static void pdb_chunk_filepath(const pdb_t *pdb, size_t chunk_index, char *buf, size_t bufsz) {
    snprintf(buf, bufsz, "%s/%s/%0*zu", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type), PDB_CHUNK_PAD, chunk_index);
}

bool pdb_save_chunk(const pdb_t *pdb, size_t chunk_index) {
    assert(pdb != NULL);
    assert(chunk_index < pdb->total_chunks);

    char filepath[1024];
    pdb_chunk_filepath(pdb, chunk_index, filepath, sizeof(filepath));

    char dirpath[1024];
    snprintf(dirpath, sizeof(dirpath), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));
    mkdir_parents(dirpath);

    FILE *f = fopen(filepath, "wb");
    if (f == NULL)
        return false;

    uint32_t header[3];
    header[0] = (uint32_t)chunk_index;
    header[1] = (uint32_t)pdb_chunk_entry_count(pdb, chunk_index);
    header[2] = (uint32_t)pdb->entry_size;

    if (fwrite(header, sizeof(header), 1, f) != 1) {
        fclose(f);
        return false;
    }

    size_t offset = chunk_index * pdb->entries_per_chunk * pdb->entry_size;
    size_t bytes  = (size_t)header[1] * pdb->entry_size;

    if (fwrite((const char *)pdb->data + offset, 1, bytes, f) != bytes) {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

bool pdb_load_chunk(pdb_t *pdb, size_t chunk_index) {
    assert(pdb != NULL);
    assert(chunk_index < pdb->total_chunks);

    char filepath[1024];
    pdb_chunk_filepath(pdb, chunk_index, filepath, sizeof(filepath));

    FILE *f = fopen(filepath, "rb");
    if (f == NULL)
        return false;

    uint32_t header[3];
    if (fread(header, sizeof(header), 1, f) != 1) {
        fclose(f);
        return false;
    }

    if (header[2] != (uint32_t)pdb->entry_size) {
        fclose(f);
        return false;
    }

    size_t offset = chunk_index * pdb->entries_per_chunk * pdb->entry_size;
    size_t bytes  = (size_t)header[1] * pdb->entry_size;

    if (fread((char *)pdb->data + offset, 1, bytes, f) != bytes) {
        fclose(f);
        return false;
    }

    fclose(f);
    return true;
}

bool pdb_load_from_disk(pdb_t *pdb) {
    assert(pdb != NULL);

    char dirpath[1024];
    snprintf(dirpath, sizeof(dirpath), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));

    struct stat st;
    if (stat(dirpath, &st) != 0) {
        pdb->loaded        = true;
        pdb->loaded_chunks = 0;
        return true;
    }

    if (!pdb_load_chunk(pdb, 0)) {
        pdb->loaded        = true;
        pdb->loaded_chunks = 0;
        return true;
    }

    pdb->loaded        = true;
    pdb->loaded_chunks = 1;
    return true;
}

void pdb_attach_to_solver(solver_context_t *ctx) {
    assert(ctx != NULL);

    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (pdb == NULL) {
        pdb_build_2x2_mdist();
        pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    }
    if (pdb != NULL && !pdb->loaded)
        pdb_load_from_disk(pdb);

    ctx->pdb = pdb;
}
