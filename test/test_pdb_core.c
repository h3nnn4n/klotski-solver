#include <stdint.h>
#include <string.h>
#include <unity.h>

#include "pdb.h"
#include "solver.h"

void setUp(void) {}
void tearDown(void) {}

void test_pdb_alloc_and_free(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 100, 4);

    TEST_ASSERT_NOT_NULL(pdb);
    TEST_ASSERT_EQUAL(PDB_BIG_SQUARE_MANHATTAN, pdb->type);
    TEST_ASSERT_EQUAL(100, pdb->total_entries);
    TEST_ASSERT_EQUAL(4, pdb->entry_size);
    TEST_ASSERT_EQUAL(PDB_CHUNK_SIZE / 4, pdb->entries_per_chunk);
    TEST_ASSERT_FALSE(pdb->loaded);
    TEST_ASSERT_EQUAL(0, pdb->loaded_chunks);
    TEST_ASSERT_NOT_NULL(pdb->data);

    size_t expected_chunks = (100 + pdb->entries_per_chunk - 1) / pdb->entries_per_chunk;
    TEST_ASSERT_EQUAL(expected_chunks, pdb->total_chunks);

    pdb_destroy(pdb);
}

void test_pdb_alloc_single_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 1, PDB_CHUNK_SIZE);

    TEST_ASSERT_EQUAL(1, pdb->entries_per_chunk);
    TEST_ASSERT_EQUAL(1, pdb->total_chunks);

    pdb_destroy(pdb);
}

void test_pdb_data_zeroed_on_alloc(void) {
    pdb_t *pdb     = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 4, 4);
    uint8_t *bytes = (uint8_t *)pdb->data;

    for (size_t i = 0; i < 16; i++)
        TEST_ASSERT_EQUAL(0, bytes[i]);

    pdb_destroy(pdb);
}

void test_pdb_set_get_entry_roundtrip(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, sizeof(uint32_t));

    uint32_t val  = 42;
    uint32_t out  = 0;
    pdb_set_entry(pdb, 3, &val);
    pdb_get_entry(pdb, 3, &out);

    TEST_ASSERT_EQUAL(42, out);

    pdb_destroy(pdb);
}

void test_pdb_set_get_multiple_entries(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 100, sizeof(uint32_t));

    for (uint32_t i = 0; i < 100; i++)
        pdb_set_entry(pdb, i, &i);

    for (uint32_t i = 0; i < 100; i++) {
        uint32_t out = 0;
        pdb_get_entry(pdb, i, &out);
        TEST_ASSERT_EQUAL(i, out);
    }

    pdb_destroy(pdb);
}

void test_pdb_set_get_boundary_entries(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 100, sizeof(uint32_t));

    uint32_t v0   = 111;
    uint32_t v99  = 999;
    uint32_t v50  = 555;
    uint32_t out0 = 0, out99 = 0, out50 = 0;

    pdb_set_entry(pdb, 0, &v0);
    pdb_set_entry(pdb, 99, &v99);
    pdb_set_entry(pdb, 50, &v50);

    pdb_get_entry(pdb, 0, &out0);
    pdb_get_entry(pdb, 99, &out99);
    pdb_get_entry(pdb, 50, &out50);

    TEST_ASSERT_EQUAL(111, out0);
    TEST_ASSERT_EQUAL(999, out99);
    TEST_ASSERT_EQUAL(555, out50);

    pdb_destroy(pdb);
}

void test_pdb_entry_size_one(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 5, 1);

    uint8_t vals[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++)
        pdb_set_entry(pdb, i, &vals[i]);

    for (int i = 0; i < 5; i++) {
        uint8_t out = 0;
        pdb_get_entry(pdb, i, &out);
        TEST_ASSERT_EQUAL(vals[i], out);
    }

    pdb_destroy(pdb);
}

void test_pdb_entry_size_two(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 3, 2);

    uint16_t v = 0xABCD;
    pdb_set_entry(pdb, 1, &v);

    uint16_t out = 0;
    pdb_get_entry(pdb, 1, &out);
    TEST_ASSERT_EQUAL(0xABCD, out);

    pdb_destroy(pdb);
}

void test_pdb_entry_size_eight(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 2, 8);

    uint64_t v = UINT64_C(0xDEADBEEFCAFEBABE);
    pdb_set_entry(pdb, 0, &v);

    uint64_t out = 0;
    pdb_get_entry(pdb, 0, &out);
    TEST_ASSERT_EQUAL_HEX64(UINT64_C(0xDEADBEEFCAFEBABE), out);

    pdb_destroy(pdb);
}

void test_pdb_entries_dont_overlap(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 4, 4);

    uint32_t vals[] = {0xAAAAAAAA, 0xBBBBBBBB, 0xCCCCCCCC, 0xDDDDDDDD};
    for (int i = 0; i < 4; i++)
        pdb_set_entry(pdb, i, &vals[i]);

    for (int i = 0; i < 4; i++) {
        uint32_t out = 0;
        pdb_get_entry(pdb, i, &out);
        TEST_ASSERT_EQUAL_HEX32(vals[i], out);
    }

    pdb_destroy(pdb);
}

void test_pdb_global_registry(void) {
    TEST_ASSERT_NULL(pdb_get_global(PDB_BIG_SQUARE_MANHATTAN));

    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 1, 1);
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, pdb);

    TEST_ASSERT_EQUAL_PTR(pdb, pdb_get_global(PDB_BIG_SQUARE_MANHATTAN));

    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
    TEST_ASSERT_NULL(pdb_get_global(PDB_BIG_SQUARE_MANHATTAN));

    pdb_destroy(pdb);
}

void test_solver_context_pdb_null(void) {
    solver_context_t *ctx = build_solver_context();

    TEST_ASSERT_NOT_NULL(ctx);
    TEST_ASSERT_NOT_NULL(ctx->board);
    TEST_ASSERT_NULL(ctx->pdb);

    destroy_solver_context(ctx);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pdb_alloc_and_free);
    RUN_TEST(test_pdb_alloc_single_chunk);
    RUN_TEST(test_pdb_data_zeroed_on_alloc);
    RUN_TEST(test_pdb_set_get_entry_roundtrip);
    RUN_TEST(test_pdb_set_get_multiple_entries);
    RUN_TEST(test_pdb_set_get_boundary_entries);
    RUN_TEST(test_pdb_entry_size_one);
    RUN_TEST(test_pdb_entry_size_two);
    RUN_TEST(test_pdb_entry_size_eight);
    RUN_TEST(test_pdb_entries_dont_overlap);
    RUN_TEST(test_pdb_global_registry);
    RUN_TEST(test_solver_context_pdb_null);

    return UNITY_END();
}
