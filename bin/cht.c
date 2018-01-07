#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "chain.h"
#include "chaintool_genesis.h"

void usage() {
    printf("USAGE: %s [-h] COMMAND [ARGS...]\n", PROG_NAME);
}

void help() {
    usage();
    printf("\n");
    printf("COMMANDS:\n");
    printf("    genesis    Create a genesis block.\n");
    printf("\n");
    printf("OPTIONS:\n");
    printf("    -h         Print help message.\n");
}

int main(int argc, const char *argv[]) {
    if (chain_init() != 0) {
        printf("Could not initialize chain.\n");
        return CHAIN_RESULT_ERROR;
    }

    int option;
    char *command;
    while((option = getopt(argc, (char* const*) argv, "h")) != -1) {
        switch(option) {
            case 'h': help(); return CHAIN_RESULT_ERROR;
            default:  usage(); return CHAIN_RESULT_ERROR;
        }
    }

    if (optind >= argc) {
        usage();
        return CHAIN_RESULT_ERROR;
    }

    command = (char*) argv[optind];
    if (strcmp(command, "genesis") == 0) return chaintool_genesis(argc, argv);

    usage();
    return CHAIN_RESULT_ERROR;
}
