#include <stdio.h>
#include <sodium.h>
#include <getopt.h>
#include <string.h>
#include <hiredis/hiredis.h>

#include "chain.h"
#include "chain_redis.h"
#include "chaintool_genesis.h"

void chaintool_genesis_usage() {
    printf("USAGE: %s [ARGS...] genesis [-h] [-d DATAVAL] \n", PROG_NAME);
}

void chaintool_genesis_help() {
    chaintool_genesis_usage();
    printf("\n");
    printf("OPTIONS:\n");
    printf("    -h            Print help message.\n");
    printf("    -d DATAVAL    Change the block's data value.\n");
    printf("\n");
}

int chaintool_genesis(int argc, const char *argv[]) {
    int option;
    block_t b;
    char data_val[4096] = "Genesis Block.";
    char hash[CHAIN_DEFAULT_HASH_CHARS + 1];
    unsigned char nonce[CHAIN_DEFAULT_HASH_BYTES];

    optind++; // skip the subcommand.
    while((option = getopt(argc, (char* const*) argv, "hd:")) != -1) {
        switch(option) {
            case 'h': chaintool_genesis_usage(); return CHAIN_RESULT_ERROR;
            case 'd': strcpy(data_val, optarg); break;
            default:  chaintool_genesis_usage(); return CHAIN_RESULT_ERROR;
        }
    }

    printf("\n");
    chain_redis_server_t server;
    chain_redis_server_init(&server);

    redisContext *ctx = redisConnect("127.0.0.1", 6379);
    if (ctx->err) {
        printf("Error: %s\n", ctx->errstr);
        return 1;
    }

    printf("Creating new chain.\n\n");
    randombytes_buf(nonce, sizeof nonce);
    sodium_bin2hex(hash, sizeof hash, nonce, sizeof nonce);
    block_init(&b, 0L, 0, hash, data_val);
    block_set_time(&b);
    block_compute_hash(&b);
    block_print(&b);
    block_flush(&b, ctx);
    chain_redis_server_close(&server);

    return CHAIN_RESULT_OK;
}