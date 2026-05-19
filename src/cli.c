#include "cli.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pdb.h"

static void print_help(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("  --help                Show this help\n");
    printf("  --gui                 Launch interactive GUI\n");
    printf("  --build-cache         Build all PDB caches (skip if exists)\n");
    printf("  --rebuild-cache       Delete and rebuild all PDB caches\n");
    printf("  --delete-cache        Delete all PDB caches\n");
    printf("  --verify-cache        Verify all PDB caches on disk\n");
    printf("  --list-pdbs           List all PDB types and their stats\n");
    printf("  --pdb=<name>          Limit operation to one PDB type\n");
    printf("\n");
}

typedef struct {
    struct timespec start;
    struct timespec last_print;
    bool            header_printed;
} progress_state_t;

static double elapsed_since(const struct timespec *start) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (double)(now.tv_sec - start->tv_sec) + (double)(now.tv_nsec - start->tv_nsec) / 1e9;
}

static void cli_progress_callback(pdb_type_t type, size_t current, size_t total, void *user_data) {
    progress_state_t      *state = (progress_state_t *)user_data;
    const pdb_type_info_t *info  = pdb_get_type_info(type);

    double now_elapsed = elapsed_since(&state->start);
    bool   is_last     = (current >= total);
    bool   is_first    = (current <= 1);

    if (!is_first && !is_last) {
        double since_print = elapsed_since(&state->last_print);
        if (since_print < 10.0)
            return;
    }

    if (!state->header_printed) {
        printf("%-22s %8s %10s %10s %9s\n", "PDB", "Progress", "Nodes/s", "Total", "Elapsed");
        state->header_printed = true;
    }

    double pct         = total > 0 ? (double)current / (double)total * 100.0 : 100.0;
    double nodes_per_s = now_elapsed > 0.0 ? (double)current / now_elapsed : 0.0;

    printf("\r%-22s %7.1f%% %10.0f %10zu %8.2fs", info->name, pct, nodes_per_s, total, now_elapsed);

    if (is_last)
        printf("\n");

    fflush(stdout);
    clock_gettime(CLOCK_MONOTONIC, &state->last_print);
}

cli_args_t cli_parse(int argc, char **argv) {
    cli_args_t args = {CLI_HELP, NULL};

    optind = 1;

    static struct option long_opts[] = {{"help", no_argument, NULL, 'h'},
                                        {"gui", no_argument, NULL, 'g'},
                                        {"build-cache", no_argument, NULL, 'b'},
                                        {"rebuild-cache", no_argument, NULL, 'r'},
                                        {"delete-cache", no_argument, NULL, 'd'},
                                        {"verify-cache", no_argument, NULL, 'v'},
                                        {"list-pdbs", no_argument, NULL, 'l'},
                                        {"pdb", required_argument, NULL, 'p'},
                                        {NULL, 0, NULL, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "h", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'h': args.action = CLI_HELP; break;
            case 'g': args.action = CLI_GUI; break;
            case 'b': args.action = CLI_BUILD; break;
            case 'r': args.action = CLI_REBUILD; break;
            case 'd': args.action = CLI_DELETE; break;
            case 'v': args.action = CLI_VERIFY; break;
            case 'l': args.action = CLI_LIST; break;
            case 'p': args.pdb_filter = optarg; break;
            default: fprintf(stderr, "Unknown option. Use --help for usage.\n"); exit(1);
        }
    }

    if (args.pdb_filter != NULL && args.action == CLI_HELP) {
        fprintf(stderr, "--pdb requires an action flag.\n");
        exit(1);
    }

    return args;
}

int cli_run(int argc, char **argv) {
    if (argc <= 1) {
        print_help(argv[0]);
        return 0;
    }

    cli_args_t args = cli_parse(argc, argv);

    if (args.action == CLI_HELP) {
        print_help(argv[0]);
        return 0;
    }

    if (args.action == CLI_GUI) {
        return 0;
    }

    if (args.action == CLI_LIST) {
        pdb_list_all();
        return 0;
    }

    if (args.pdb_filter != NULL) {
        switch (args.action) {
            case CLI_BUILD: pdb_build_named(args.pdb_filter, NULL, NULL); break;
            case CLI_REBUILD: pdb_rebuild_named(args.pdb_filter, NULL, NULL); break;
            case CLI_DELETE:
                pdb_delete_cache_named(args.pdb_filter);
                printf("Deleted cache/pdb/%s\n", args.pdb_filter);
                break;
            case CLI_VERIFY: pdb_verify_named(args.pdb_filter); break;
            default: break;
        }
        return 0;
    }

    progress_state_t prog_state;
    memset(&prog_state, 0, sizeof(prog_state));
    clock_gettime(CLOCK_MONOTONIC, &prog_state.start);
    clock_gettime(CLOCK_MONOTONIC, &prog_state.last_print);

    switch (args.action) {
        case CLI_BUILD: pdb_build_all(cli_progress_callback, &prog_state); break;
        case CLI_REBUILD:
            printf("Deleting all PDB caches...\n");
            pdb_rebuild_all(cli_progress_callback, &prog_state);
            break;
        case CLI_DELETE:
            pdb_delete_cache_all();
            printf("Deleted all PDB caches.\n");
            break;
        case CLI_VERIFY: pdb_verify_all(); break;
        default: break;
    }

    return 0;
}
