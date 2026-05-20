#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unity.h>

#include "file_utils.h"
#include "pdb.h"
#include "pdb_2x2.h"
#include "solver.h"

#define TEST_DIR "cache/pdb/_test_io"

void setUp(void) {
    file_utils_remove_directory(TEST_DIR);
    file_utils_remove_directory("cache/pdb/pdb_big_square_manhattan");
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
}

void tearDown(void) {
    file_utils_remove_directory(TEST_DIR);
    file_utils_remove_directory("cache/pdb/pdb_big_square_manhattan");
    pdb_t *old = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (old != NULL) {
        pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
        pdb_destroy(old);
    }
}

void test_save_and_load_single_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 50, sizeof(uint32_t));

    uint32_t vals[50];
    for (int i = 0; i < 50; i++) {
        vals[i] = (uint32_t)(i * 100 + 7);
        pdb_set_entry(pdb, i, &vals[i]);
    }

    TEST_ASSERT_TRUE(pdb_save_chunk(pdb, 0));

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 50, sizeof(uint32_t));
    TEST_ASSERT_TRUE(pdb_load_chunk(loaded, 0));

    for (int i = 0; i < 50; i++) {
        uint32_t out = 0;
        pdb_get_entry(loaded, i, &out);
        TEST_ASSERT_EQUAL(vals[i], out);
    }

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_save_chunk_creates_directory(void) {
    pdb_t      *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 5, 1);
    struct stat st;

    pdb_save_chunk(pdb, 0);

    TEST_ASSERT_TRUE(stat("cache/pdb/pdb_big_square_manhattan", &st) == 0);

    pdb_destroy(pdb);
}

void test_load_nonexistent_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);

    TEST_ASSERT_FALSE(pdb_load_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_load_from_disk_no_folder(void) {
    file_utils_remove_directory(TEST_DIR);

    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);

    TEST_ASSERT_TRUE(pdb_load_from_disk(pdb));
    TEST_ASSERT_TRUE(pdb->loaded);
    TEST_ASSERT_EQUAL(0, pdb->loaded_chunks);

    pdb_destroy(pdb);
}

void test_load_from_disk_with_data(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 30, sizeof(uint32_t));

    uint32_t vals[30];
    for (int i = 0; i < 30; i++) {
        vals[i] = (uint32_t)(i * 13 + 42);
        pdb_set_entry(pdb, i, &vals[i]);
    }

    pdb_save_chunk(pdb, 0);

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 30, sizeof(uint32_t));
    TEST_ASSERT_TRUE(pdb_load_from_disk(loaded));
    TEST_ASSERT_TRUE(loaded->loaded);

    for (int i = 0; i < 30; i++) {
        uint32_t out = 0;
        pdb_get_entry(loaded, i, &out);
        TEST_ASSERT_EQUAL(vals[i], out);
    }

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_attach_to_solver(void) {
    solver_context_t *ctx = build_solver_context();
    TEST_ASSERT_NULL(ctx->pdb);

    pdb_attach_to_solver(ctx);

    TEST_ASSERT_NOT_NULL(ctx->pdb);
    TEST_ASSERT_EQUAL(PDB_BIG_SQUARE_MANHATTAN, ctx->pdb->type);
    TEST_ASSERT_EQUAL(12, ctx->pdb->total_entries);

    destroy_solver_context(ctx);

    pdb_t *old = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (old != NULL) {
        pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
        pdb_destroy(old);
    }
}

void test_save_overwrites_existing(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 5, sizeof(uint32_t));

    uint32_t v = 999;
    pdb_set_entry(pdb, 0, &v);
    pdb_save_chunk(pdb, 0);

    v = 111;
    pdb_set_entry(pdb, 0, &v);
    pdb_save_chunk(pdb, 0);

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 5, sizeof(uint32_t));
    pdb_load_chunk(loaded, 0);

    uint32_t out = 0;
    pdb_get_entry(loaded, 0, &out);
    TEST_ASSERT_EQUAL(111, out);

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_save_reads_back_correct_entry_count(void) {
    pdb_t *pdb      = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 17, 1);
    uint8_t expected = 42;
    pdb_set_entry(pdb, 16, &expected);
    pdb_save_chunk(pdb, 0);

    pdb_t *loaded  = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 17, 1);
    pdb_load_chunk(loaded, 0);

    uint8_t out = 0;
    pdb_get_entry(loaded, 16, &out);
    TEST_ASSERT_EQUAL(42, out);

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_multi_chunk_save_and_load(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, sizeof(uint32_t));
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    uint32_t vals[20];
    for (int i = 0; i < 20; i++) {
        vals[i] = (uint32_t)(i * 999);
        pdb_set_entry(pdb, i, &vals[i]);
    }

    for (size_t ci = 0; ci < pdb->total_chunks; ci++)
        pdb_save_chunk(pdb, ci);

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, sizeof(uint32_t));
    loaded->entries_per_chunk = 8;
    loaded->total_chunks      = 3;
    pdb_load_from_disk(loaded);

    TEST_ASSERT_EQUAL(3, loaded->loaded_chunks);

    for (int i = 0; i < 20; i++) {
        uint32_t out = 0;
        pdb_get_entry(loaded, i, &out);
        TEST_ASSERT_EQUAL(vals[i], out);
    }

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_scan_all_present(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    for (size_t ci = 0; ci < pdb->total_chunks; ci++)
        pdb_save_chunk(pdb, ci);

    size_t count = pdb_count_existing_chunks(pdb);
    TEST_ASSERT_EQUAL(3, count);

    pdb_destroy(pdb);
}

void test_scan_partial(void) {
    pdb_t   *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    pdb_save_chunk(pdb, 0);
    pdb_save_chunk(pdb, 1);

    size_t count = pdb_count_existing_chunks(pdb);
    TEST_ASSERT_EQUAL(2, count);

    pdb_destroy(pdb);
}

void test_scan_ignores_non_chunk_files(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);
    pdb->entries_per_chunk = 5;
    pdb->total_chunks      = 2;

    pdb_save_chunk(pdb, 0);

    char other[1024];
    snprintf(other, sizeof(other), "%s/%s/notes.txt", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));
    FILE *f = fopen(other, "w");
    if (f != NULL) {
        fprintf(f, "hello");
        fclose(f);
    }

    size_t count = pdb_count_existing_chunks(pdb);
    TEST_ASSERT_EQUAL(1, count);

    pdb_destroy(pdb);
}

void test_load_from_disk_partial_state(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    pdb_save_chunk(pdb, 0);
    pdb_save_chunk(pdb, 1);

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    loaded->entries_per_chunk = 8;
    loaded->total_chunks      = 3;
    pdb_load_from_disk(loaded);

    TEST_ASSERT_EQUAL(2, loaded->loaded_chunks);

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_load_from_disk_handles_gap(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    pdb_save_chunk(pdb, 0);
    pdb_save_chunk(pdb, 2);

    pdb_t *loaded = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    loaded->entries_per_chunk = 8;
    loaded->total_chunks      = 3;
    pdb_load_from_disk(loaded);

    TEST_ASSERT_EQUAL(1, loaded->loaded_chunks);

    pdb_destroy(pdb);
    pdb_destroy(loaded);
}

void test_chunk_filename_padding(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 5, 1);

    pdb_save_chunk(pdb, 0);

    char path[1024];
    snprintf(path, sizeof(path), "%s/%s/%0*zu", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type), PDB_CHUNK_PAD,
             (size_t)0);
    FILE *f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    if (f != NULL)
        fclose(f);

    pdb_destroy(pdb);
}

void test_save_chunk_at_nonzero_index(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 50, 1);
    pdb->entries_per_chunk = 20;
    pdb->total_chunks      = 3;

    pdb_save_chunk(pdb, 2);

    char path[1024];
    snprintf(path, sizeof(path), "%s/%s/0002", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));
    FILE *f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    if (f != NULL)
        fclose(f);

    pdb_destroy(pdb);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_save_and_load_single_chunk);
    RUN_TEST(test_save_chunk_creates_directory);
    RUN_TEST(test_load_nonexistent_chunk);
    RUN_TEST(test_load_from_disk_no_folder);
    RUN_TEST(test_load_from_disk_with_data);
    RUN_TEST(test_attach_to_solver);
    RUN_TEST(test_save_overwrites_existing);
    RUN_TEST(test_save_reads_back_correct_entry_count);

    RUN_TEST(test_multi_chunk_save_and_load);
    RUN_TEST(test_scan_all_present);
    RUN_TEST(test_scan_partial);
    RUN_TEST(test_scan_ignores_non_chunk_files);
    RUN_TEST(test_load_from_disk_partial_state);
    RUN_TEST(test_load_from_disk_handles_gap);
    RUN_TEST(test_chunk_filename_padding);
    RUN_TEST(test_save_chunk_at_nonzero_index);

    return UNITY_END();
}
