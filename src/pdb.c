#include "pdb.h"
#include "file_utils.h"
#include "pdb_2x2.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "solver.h"

static pdb_t *loaded_pdbs[PDB_TYPE_COUNT];

static pdb_type_info_t info_table[PDB_TYPE_COUNT] = {
    [PDB_BIG_SQUARE_MANHATTAN] =
        {
            .type          = PDB_BIG_SQUARE_MANHATTAN,
            .name          = "big_square_manhattan",
            .pieces        = "Big square (2x2)",
            .total_entries = 12,
            .entry_size    = 1,
        },
};

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

static void mkdir_parents(const char *path) { file_utils_mkdir_parents(path); }

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

    pdb->loaded        = true;
    pdb->loaded_chunks = 0;

    size_t valid_chunks = pdb_count_existing_chunks(pdb);

    for (size_t ci = 0; ci < valid_chunks; ci++) {
        if (!pdb_load_chunk(pdb, ci))
            break;
        pdb->loaded_chunks = ci + 1;
    }

    return true;
}

size_t pdb_count_existing_chunks(const pdb_t *pdb) {
    assert(pdb != NULL);

    char dirpath[1024];
    snprintf(dirpath, sizeof(dirpath), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));

    DIR *dir = opendir(dirpath);
    if (dir == NULL)
        return 0;

    bool found[4096];
    memset(found, 0, sizeof(found));

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strlen(entry->d_name) != (size_t)PDB_CHUNK_PAD)
            continue;

        char *endptr;
        long  idx = strtol(entry->d_name, &endptr, 10);
        if (idx < 0 || (size_t)idx >= pdb->total_chunks)
            continue;
        if (*endptr != '\0')
            continue;

        char filepath[1024];
        pdb_chunk_filepath(pdb, (size_t)idx, filepath, sizeof(filepath));

        FILE *f = fopen(filepath, "rb");
        if (f == NULL)
            continue;

        uint32_t header[3];
        if (fread(header, sizeof(header), 1, f) != 1) {
            fclose(f);
            continue;
        }

        if (header[2] != (uint32_t)pdb->entry_size) {
            fclose(f);
            continue;
        }

        long fsize;
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        fclose(f);

        size_t expected_size = (long)(sizeof(header) + (size_t)header[1] * pdb->entry_size);
        if ((long)expected_size != fsize)
            continue;

        found[idx] = true;
    }
    closedir(dir);

    size_t count = 0;
    for (size_t i = 0; i < pdb->total_chunks && found[i]; i++)
        count++;

    return count;
}

bool pdb_validate_chunk(const pdb_t *pdb, size_t chunk_index) {
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

    if (header[0] != (uint32_t)chunk_index) {
        fclose(f);
        return false;
    }

    if (header[2] != (uint32_t)pdb->entry_size) {
        fclose(f);
        return false;
    }

    size_t expected_count = pdb_chunk_entry_count(pdb, chunk_index);
    if (header[1] != (uint32_t)expected_count) {
        fclose(f);
        return false;
    }

    fseek(f, 0, SEEK_END);
    long   fsize         = ftell(f);
    size_t expected_size = sizeof(header) + expected_count * pdb->entry_size;
    fclose(f);

    return (long)expected_size == fsize;
}

bool pdb_validate_all(const pdb_t *pdb) {
    assert(pdb != NULL);

    for (size_t ci = 0; ci < pdb->total_chunks; ci++) {
        if (!pdb_validate_chunk(pdb, ci))
            return false;
    }

    return true;
}

void pdb_attach_to_solver(solver_context_t *ctx) {
    assert(ctx != NULL);

    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (pdb == NULL) {
        pdb_build_2x2_mdist(NULL, NULL);
        pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    }
    if (pdb != NULL && !pdb->loaded)
        pdb_load_from_disk(pdb);

    ctx->pdb = pdb;
}

const pdb_type_info_t *pdb_get_type_info(pdb_type_t type) {
    assert(type < PDB_TYPE_COUNT);
    return &info_table[type];
}

pdb_type_t pdb_find_type_by_name(const char *name) {
    for (int i = 0; i < PDB_TYPE_COUNT; i++) {
        if (strcmp(info_table[i].name, name) == 0)
            return (pdb_type_t)i;
    }
    return PDB_TYPE_COUNT;
}

size_t pdb_get_total_size_on_disk(pdb_type_t type) {
    const pdb_type_info_t *info  = pdb_get_type_info(type);
    size_t                 total = 0;

    char dirpath[1024];
    snprintf(dirpath, sizeof(dirpath), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(type));

    DIR *dir = opendir(dirpath);
    if (dir == NULL)
        return 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char filepath[1280];
        snprintf(filepath, sizeof(filepath), "%s/%s", dirpath, entry->d_name);

        struct stat st;
        if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode))
            total += (size_t)st.st_size;
    }
    closedir(dir);
    (void)info;
    return total;
}

static void pdb_rmrf_type(pdb_type_t type) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(type));
    file_utils_remove_directory(path);
}

void pdb_build_all(pdb_progress_fn progress, void *user_data) {
    for (int i = 0; i < PDB_TYPE_COUNT; i++)
        pdb_build_named(info_table[i].name, progress, user_data);
}

void pdb_rebuild_all(pdb_progress_fn progress, void *user_data) {
    pdb_delete_cache_all();
    pdb_build_all(progress, user_data);
}

void pdb_delete_cache_all(void) {
    for (int i = 0; i < PDB_TYPE_COUNT; i++)
        pdb_rmrf_type((pdb_type_t)i);
}

void pdb_verify_all(void) {
    for (int i = 0; i < PDB_TYPE_COUNT; i++)
        pdb_verify_named(info_table[i].name);
}

void pdb_list_all(void) {
    printf("%-22s %-20s %8s %11s %10s\n", "Name", "Pieces", "Entries", "Entry Size", "On Disk");
    printf("%-22s %-20s %8s %11s %10s\n", "--------------------", "--------------------", "--------", "-----------",
           "----------");

    for (int i = 0; i < PDB_TYPE_COUNT; i++) {
        const pdb_type_info_t *info = &info_table[i];
        size_t                 size = pdb_get_total_size_on_disk((pdb_type_t)i);

        const char *size_unit = "B";
        double      size_val  = (double)size;
        if (size_val >= 1024.0) {
            size_val /= 1024.0;
            size_unit = "KB";
        }
        if (size_val >= 1024.0) {
            size_val /= 1024.0;
            size_unit = "MB";
        }
        if (size_val >= 1024.0) {
            size_val /= 1024.0;
            size_unit = "GB";
        }

        char size_str[32];
        if (size_val >= 10.0)
            snprintf(size_str, sizeof(size_str), "%.0f %s", size_val, size_unit);
        else
            snprintf(size_str, sizeof(size_str), "%.1f %s", size_val, size_unit);

        const char *entry_unit = "B";
        double      entry_val  = (double)info->entry_size;
        if (entry_val >= 1024.0) {
            entry_val /= 1024.0;
            entry_unit = "KB";
        }

        char entry_str[32];
        if (entry_val >= 1.0)
            snprintf(entry_str, sizeof(entry_str), "%.0f %s", entry_val, entry_unit);
        else
            snprintf(entry_str, sizeof(entry_str), "%.0f B", entry_val);

        printf("%-22s %-20s %8zu %11s %10s\n", info->name, info->pieces, info->total_entries, entry_str, size_str);
    }
}

void pdb_build_named(const char *name, pdb_progress_fn progress, void *user_data) {
    pdb_type_t type = pdb_find_type_by_name(name);
    if (type == PDB_TYPE_COUNT) {
        fprintf(stderr, "Unknown PDB type: %s\n", name);
        return;
    }

    if (type == PDB_BIG_SQUARE_MANHATTAN)
        pdb_build_2x2_mdist(progress, user_data);
}

void pdb_rebuild_named(const char *name, pdb_progress_fn progress, void *user_data) {
    pdb_type_t type = pdb_find_type_by_name(name);
    if (type == PDB_TYPE_COUNT)
        return;
    pdb_rmrf_type(type);
    pdb_build_named(name, progress, user_data);
}

void pdb_delete_cache_named(const char *name) {
    pdb_type_t type = pdb_find_type_by_name(name);
    if (type == PDB_TYPE_COUNT)
        return;
    pdb_rmrf_type(type);
}

void pdb_verify_named(const char *name) {
    pdb_type_t             type = pdb_find_type_by_name(name);
    const pdb_type_info_t *info = (type < PDB_TYPE_COUNT) ? pdb_get_type_info(type) : NULL;

    if (type == PDB_TYPE_COUNT || info == NULL) {
        fprintf(stderr, "Unknown PDB type: %s\n", name);
        return;
    }

    pdb_t *pdb = pdb_build(type, info->total_entries, info->entry_size);
    pdb_load_from_disk(pdb);

    printf("%-22s ", info->name);

    if (pdb->loaded_chunks == 0 && info->total_entries > 0) {
        printf("MISSING    %6zu %9zu\n", pdb->total_chunks, info->total_entries);
        pdb_destroy(pdb);
        return;
    }

    bool all_good = true;

    for (size_t ci = 0; ci < pdb->total_chunks; ci++) {
        if (!pdb_validate_chunk(pdb, ci)) {
            all_good = false;
            printf("INVALID     %6zu %9zu (chunk %zu)", pdb->total_chunks, info->total_entries, ci);

            char filepath[1024];
            pdb_chunk_filepath(pdb, ci, filepath, sizeof(filepath));
            FILE *f = fopen(filepath, "rb");
            if (f == NULL) {
                printf(" missing\n");
            } else {
                uint32_t header[3] = {0, 0, 0};
                if (fread(header, sizeof(header), 1, f) != 1) {
                    printf(" header unreadable\n");
                    fclose(f);
                    break;
                }
                fclose(f);

                if (header[2] != (uint32_t)info->entry_size)
                    printf(" wrong entry_size (got %u expected %zu)\n", header[2], info->entry_size);
                else if (header[1] != (uint32_t)pdb_chunk_entry_count(pdb, ci))
                    printf(" wrong entry_count (got %u expected %zu)\n", header[1], pdb_chunk_entry_count(pdb, ci));
                else
                    printf(" wrong size\n");
            }
            break;
        }
    }

    if (all_good)
        printf("VALID       %6zu %9zu\n", pdb->total_chunks, info->total_entries);

    pdb_destroy(pdb);
}
