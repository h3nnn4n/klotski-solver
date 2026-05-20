#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unity.h>

#include "file_utils.h"
#include "pdb.h"
#include "pdb_2x2.h"

void setUp(void) {
    file_utils_remove_directory("cache/pdb/pdb_big_square_manhattan");
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
}

void tearDown(void) {
    file_utils_remove_directory("cache/pdb/pdb_big_square_manhattan");
    pdb_t *old = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (old != NULL) {
        pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
        pdb_destroy(old);
    }
}

void test_validate_valid_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);
    pdb_save_chunk(pdb, 0);

    TEST_ASSERT_TRUE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_validate_missing_chunk(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);

    TEST_ASSERT_FALSE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

static void write_bad_header_chunk(pdb_t *pdb, size_t chunk_index, uint32_t bad_entry_count,
                                    uint32_t bad_entry_size) {
    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s/%0*zu", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type),
             PDB_CHUNK_PAD, chunk_index);

    char dirpath[1024];
    snprintf(dirpath, sizeof(dirpath), "%s/%s", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type));
    mkdir(dirpath, 0755);

    FILE *f = fopen(filepath, "wb");

    uint32_t header[3];
    header[0] = (uint32_t)chunk_index;
    header[1] = bad_entry_count;
    header[2] = bad_entry_size;

    fwrite(header, sizeof(header), 1, f);

    size_t   bytes = (size_t)bad_entry_count * bad_entry_size;
    uint8_t *buf   = malloc(bytes);
    memset(buf, 0, bytes);
    fwrite(buf, 1, bytes, f);
    free(buf);
    fclose(f);
}

void test_validate_wrong_entry_count(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);

    write_bad_header_chunk(pdb, 0, 99, 1);

    TEST_ASSERT_FALSE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_validate_wrong_entry_size(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, sizeof(uint32_t));

    write_bad_header_chunk(pdb, 0, 10, (uint32_t)sizeof(uint16_t));

    TEST_ASSERT_FALSE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_validate_wrong_chunk_index(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, 1);
    pdb->entries_per_chunk = 5;
    pdb->total_chunks      = 2;

    write_bad_header_chunk(pdb, 0, (uint32_t)pdb_chunk_entry_count(pdb, 0), 1);

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s/%04zu", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type), (size_t)0);

    FILE *f = fopen(filepath, "r+b");
    uint32_t bad_index = 1;
    fwrite(&bad_index, sizeof(uint32_t), 1, f);
    fclose(f);

    TEST_ASSERT_FALSE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_validate_truncated_file(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 10, sizeof(uint32_t));

    write_bad_header_chunk(pdb, 0, 10, (uint32_t)sizeof(uint32_t));

    char filepath[1024];
    snprintf(filepath, sizeof(filepath), "%s/%s/%04zu", PDB_CACHE_DIR, pdb_get_type_folder_name(pdb->type), (size_t)0);

    truncate(filepath, 13);

    TEST_ASSERT_FALSE(pdb_validate_chunk(pdb, 0));

    pdb_destroy(pdb);
}

void test_validate_all_good(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    for (size_t ci = 0; ci < pdb->total_chunks; ci++)
        pdb_save_chunk(pdb, ci);

    TEST_ASSERT_TRUE(pdb_validate_all(pdb));

    pdb_destroy(pdb);
}

void test_validate_all_with_one_bad(void) {
    pdb_t *pdb = pdb_build(PDB_BIG_SQUARE_MANHATTAN, 20, 1);
    pdb->entries_per_chunk = 8;
    pdb->total_chunks      = 3;

    pdb_save_chunk(pdb, 0);
    write_bad_header_chunk(pdb, 1, 5, 1);
    pdb_save_chunk(pdb, 2);

    TEST_ASSERT_FALSE(pdb_validate_all(pdb));

    pdb_destroy(pdb);
}

void test_2x2_resume_from_disk(void) {
    pdb_build_2x2_mdist(NULL, NULL);

    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    TEST_ASSERT_NOT_NULL(pdb);
    TEST_ASSERT_EQUAL(1, pdb->loaded_chunks);

    uint8_t dist;
    pdb_get_entry(pdb, 0, &dist);
    TEST_ASSERT_EQUAL(4, dist);

    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);

    pdb_build_2x2_mdist(NULL, NULL);

    pdb_t *pdb2 = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    TEST_ASSERT_NOT_NULL(pdb2);

    uint8_t dist2;
    pdb_get_entry(pdb2, 0, &dist2);
    TEST_ASSERT_EQUAL(4, dist2);

    pdb_destroy(pdb);
}

void test_2x2_rebuild_from_scratch(void) {
    pdb_build_2x2_mdist(NULL, NULL);

    pdb_t *first = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);

    file_utils_remove_directory("cache/pdb/pdb_big_square_manhattan");

    pdb_build_2x2_mdist(NULL, NULL);

    pdb_t *second = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    TEST_ASSERT_NOT_NULL(second);

    uint8_t dist;
    pdb_get_entry(second, 10, &dist);
    TEST_ASSERT_EQUAL(0, dist);

    pdb_destroy(first);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_validate_valid_chunk);
    RUN_TEST(test_validate_missing_chunk);
    RUN_TEST(test_validate_wrong_entry_count);
    RUN_TEST(test_validate_wrong_entry_size);
    RUN_TEST(test_validate_wrong_chunk_index);
    RUN_TEST(test_validate_truncated_file);
    RUN_TEST(test_validate_all_good);
    RUN_TEST(test_validate_all_with_one_bad);
    RUN_TEST(test_2x2_resume_from_disk);
    RUN_TEST(test_2x2_rebuild_from_scratch);

    return UNITY_END();
}
