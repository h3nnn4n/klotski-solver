#include <stdint.h>
#include <unity.h>

#include "cli.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_no_args(void) {
    char *argv[] = {"klotski"};
    cli_args_t args = cli_parse(1, argv);

    TEST_ASSERT_EQUAL(CLI_HELP, args.action);
    TEST_ASSERT_NULL(args.pdb_filter);
}

void test_parse_help(void) {
    char *argv[] = {"klotski", "--help"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_HELP, args.action);
}

void test_parse_gui(void) {
    char *argv[] = {"klotski", "--gui"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_GUI, args.action);
}

void test_parse_build_cache(void) {
    char *argv[] = {"klotski", "--build-cache"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_BUILD, args.action);
}

void test_parse_rebuild_cache(void) {
    char *argv[] = {"klotski", "--rebuild-cache"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_REBUILD, args.action);
}

void test_parse_delete_cache(void) {
    char *argv[] = {"klotski", "--delete-cache"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_DELETE, args.action);
}

void test_parse_verify_cache(void) {
    char *argv[] = {"klotski", "--verify-cache"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_VERIFY, args.action);
}

void test_parse_list_pdbs(void) {
    char *argv[] = {"klotski", "--list-pdbs"};
    cli_args_t args = cli_parse(2, argv);

    TEST_ASSERT_EQUAL(CLI_LIST, args.action);
}

void test_parse_pdb_filter(void) {
    char *argv[] = {"klotski", "--pdb=big_square_manhattan", "--build-cache"};
    cli_args_t args = cli_parse(3, argv);

    TEST_ASSERT_EQUAL(CLI_BUILD, args.action);
    TEST_ASSERT_EQUAL_STRING("big_square_manhattan", args.pdb_filter);
}

void test_parse_last_action_wins(void) {
    char *argv[] = {"klotski", "--build-cache", "--verify-cache"};
    cli_args_t args = cli_parse(3, argv);

    TEST_ASSERT_EQUAL(CLI_VERIFY, args.action);
}

void test_parse_pdb_with_space(void) {
    char *argv[] = {"klotski", "--pdb", "big_square_manhattan", "--build-cache"};
    cli_args_t args = cli_parse(4, argv);

    TEST_ASSERT_EQUAL(CLI_BUILD, args.action);
    TEST_ASSERT_EQUAL_STRING("big_square_manhattan", args.pdb_filter);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_parse_no_args);
    RUN_TEST(test_parse_help);
    RUN_TEST(test_parse_gui);
    RUN_TEST(test_parse_build_cache);
    RUN_TEST(test_parse_rebuild_cache);
    RUN_TEST(test_parse_delete_cache);
    RUN_TEST(test_parse_verify_cache);
    RUN_TEST(test_parse_list_pdbs);
    RUN_TEST(test_parse_pdb_filter);
    RUN_TEST(test_parse_last_action_wins);
    RUN_TEST(test_parse_pdb_with_space);

    return UNITY_END();
}
