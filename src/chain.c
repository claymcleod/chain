#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include <hiredis/hiredis.h>

#include "chain.h"

int chain_init() {
    return sodium_init();
}

int block_init(
    block_t *b,
    unsigned long index, 
    char *timestamp,
    char *previousHash, 
    char *data
) {
    b->index = index;
    b->timestamp = timestamp;

    b->currentHash = (char*) malloc((CHAIN_DEFAULT_HASH_CHARS + 1) * sizeof(char));
    if (b->currentHash == NULL) return CHAIN_RESULT_ERROR;
    for (int i = 0; i < CHAIN_DEFAULT_HASH_CHARS; i++) {
        b->currentHash[i] = '0';
    }
    b->currentHash[CHAIN_DEFAULT_HASH_CHARS] = '\0';

    const int PREVIOUS_HASH_LEN = strlen(previousHash);
    b->previousHash = (char*) malloc((PREVIOUS_HASH_LEN + 1) * sizeof(char));
    if (b->previousHash == NULL) return CHAIN_RESULT_ERROR;
    strcpy(b->previousHash, previousHash);
    b->previousHash[PREVIOUS_HASH_LEN] = '\0';

    b->hash_length = CHAIN_DEFAULT_HASH_CHARS;
    b->data = (char*) malloc(strlen(data) * sizeof(char));
    if (b->data == NULL) return CHAIN_RESULT_ERROR;
    strcpy(b->data, data);

    return CHAIN_RESULT_OK;
}

void block_compute_hash(block_t *b) {
    char buffer[4096];
    char hex[CHAIN_DEFAULT_HASH_CHARS + 1];
    unsigned char hash[CHAIN_DEFAULT_HASH_BYTES];

    crypto_generichash_state state;
    crypto_generichash_init(&state, NULL, 0, CHAIN_DEFAULT_HASH_BYTES);
    
    block_get_index_str(b, buffer);
    crypto_generichash_update(&state, (const unsigned char*) buffer, strlen(buffer));
    crypto_generichash_update(&state, (const unsigned char*) b->timestamp, strlen(buffer));
    crypto_generichash_update(&state, (const unsigned char*) b->data, strlen(b->data));
    crypto_generichash_final(&state, hash, sizeof hash);
    sodium_bin2hex(hex, sizeof hex, hash, sizeof hash);
    strcpy(b->currentHash, hex);
}

void block_set_time(block_t *b) {
    time_t now;
    char buffer[4096];

    now = time(NULL);
    strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M:%S", localtime(&now));

    int timestamp_length = strlen(buffer) + 1;
    b->timestamp = malloc(timestamp_length * sizeof(char));
    strcpy(b->timestamp, buffer);
    b->timestamp[timestamp_length] = '\0';
}

void block_get_index_str(block_t *b, char *buffer) {
    sprintf(buffer, "%lu", b->index);
}

void block_print(block_t *b) {
    printf("== Block %lu ==\n", b->index);
    printf("  [*] Timestamp:     %s\n", b->timestamp);
    printf("  [*] Data:          %s\n", b->data);
    printf("  [*] Current Hash:  %s\n", b->currentHash);
    printf("  [*] Previous Hash: %s\n", b->previousHash);
    printf("  [*] Hash length:   %i\n", b->hash_length);
}

void block_free(block_t *b) {
    if(b->timestamp != NULL) free(b->timestamp);
    if(b->currentHash != NULL) free(b->currentHash);
    if(b->previousHash != NULL) free(b->previousHash);
    if(b->data != NULL) free(b->data);

    b->index = 0;
    b->hash_length = 0;
    b->timestamp = NULL;
    b->currentHash = NULL;
    b->previousHash = NULL;
    b->data = NULL;
}

void block_flush(block_t *b, redisContext *ctx) {
    char buffer[4096];
    block_get_index_str(b, buffer);
    redisCommand(ctx, "SET block:%s:timestamp %s", buffer, b->timestamp);
    redisCommand(ctx, "SET block:%s:currentHash %s", buffer, b->currentHash);
    redisCommand(ctx, "SET block:%s:previousHash %s", buffer, b->previousHash);
    redisCommand(ctx, "SET block:%s:data %s", buffer, b->data);
    redisCommand(ctx, "SET block:%s:hash_length %i", buffer, b->hash_length);
}