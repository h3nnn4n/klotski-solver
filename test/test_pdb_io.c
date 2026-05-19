#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unity.h>

#include "pdb.h"
#include "pdb_2x2.h"
#include "solver.h"

#define TEST_DIR "cache/pdb/_test_io"

static void rmrf(const char *path) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", path);
    system(cmd);
}

void setUp(void) {
    rmrf(TEST_DIR);
    rmrf("cache/pdb/pdb_2x2_mdist");
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
}

void tearDown(void) {
    rmrf(TEST_DIR);
    rmrf("cache/pdb/pdb_2x2_mdist");
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

    TEST_ASSERT_TRUE(stat("cache/pdb/pdb_2x2_mdist", &st) == 0);

    pdb_destroy(pdb);
}

void test_load_nonexistent_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);

    TEST_ASSERT_FALSE(pdb_load_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_load_from_disk_no_folder(void) {
    rmrf(TEST_DIR);

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

    return UNITY_END();
}
