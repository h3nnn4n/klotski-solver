#include <stdint.h>
#include <stdlib.h>
#include <unity.h>

#include "pdb.h"
#include "pdb_2x2.h"

void setUp(void) {
    pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
}

void tearDown(void) {
    pdb_t *old = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    if (old != NULL) {
        pdb_set_global(PDB_BIG_SQUARE_MANHATTAN, NULL);
        pdb_destroy(old);
    }
}

static uint8_t expected_dist(int x, int y) {
    return (uint8_t)(abs(x - 1) + abs(y - 3));
}

void test_build_2x2_runs(void) {
    pdb_build_2x2_mdist();

    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);
    TEST_ASSERT_NOT_NULL(pdb);
    TEST_ASSERT_EQUAL(PDB_BIG_SQUARE_MANHATTAN, pdb->type);
    TEST_ASSERT_EQUAL(12, pdb->total_entries);
    TEST_ASSERT_EQUAL(1, pdb->entry_size);
}

void test_2x2_goal_distance_zero(void) {
    pdb_build_2x2_mdist();
    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    uint8_t dist = 0;
    size_t  idx  = 1 + 3 * 3; // x=1, y=3 = goal
    pdb_get_entry(pdb, idx, &dist);
    TEST_ASSERT_EQUAL(0, dist);
}

void test_2x2_corner_distances(void) {
    pdb_build_2x2_mdist();
    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    uint8_t d00 = 0, d20 = 0, d03 = 0, d23 = 0;
    pdb_get_entry(pdb, 0 + 0 * 3, &d00);
    pdb_get_entry(pdb, 2 + 0 * 3, &d20);
    pdb_get_entry(pdb, 0 + 3 * 3, &d03);
    pdb_get_entry(pdb, 2 + 3 * 3, &d23);

    TEST_ASSERT_EQUAL(4, d00);
    TEST_ASSERT_EQUAL(4, d20);
    TEST_ASSERT_EQUAL(1, d03);
    TEST_ASSERT_EQUAL(1, d23);
}

void test_2x2_all_entries_correct(void) {
    pdb_build_2x2_mdist();
    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 3; x++) {
            size_t idx = x + y * 3;
            uint8_t dist;
            pdb_get_entry(pdb, idx, &dist);
            TEST_ASSERT_EQUAL_MESSAGE(expected_dist(x, y), dist, "wrong Manhattan distance");
        }
    }
}

void test_2x2_rebuild_no_leak(void) {
    pdb_build_2x2_mdist();
    pdb_t *first = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    TEST_ASSERT_NOT_NULL(first);
    uint8_t val = 99;
    pdb_set_entry(first, 0, &val);

    pdb_build_2x2_mdist();
    pdb_t *second = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NOT_EQUAL(first, second);

    uint8_t dist;
    pdb_get_entry(second, 0, &dist);
    TEST_ASSERT_EQUAL(expected_dist(0, 0), dist);
}

void test_2x2_every_entry_is_valid_distance(void) {
    pdb_build_2x2_mdist();
    pdb_t *pdb = pdb_get_global(PDB_BIG_SQUARE_MANHATTAN);

    for (size_t i = 0; i < 12; i++) {
        uint8_t dist;
        pdb_get_entry(pdb, i, &dist);
        TEST_ASSERT_TRUE_MESSAGE(dist <= 5, "distance exceeds max Manhattan on 4x5 board");
    }
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_build_2x2_runs);
    RUN_TEST(test_2x2_goal_distance_zero);
    RUN_TEST(test_2x2_corner_distances);
    RUN_TEST(test_2x2_all_entries_correct);
    RUN_TEST(test_2x2_rebuild_no_leak);
    RUN_TEST(test_2x2_every_entry_is_valid_distance);

    return UNITY_END();
}
