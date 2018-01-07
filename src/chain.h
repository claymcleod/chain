#ifndef CHAIN_H
#define CHAIN_H

#include <time.h>
#include <hiredis/hiredis.h>

#define PROG_NAME "cht"
#define PROG_VERSION "1.0.0"

#define CHAIN_RESULT_OK 0
#define CHAIN_RESULT_ERROR 1

#define CHAIN_DEFAULT_HASH_BYTES crypto_generichash_BYTES
#define CHAIN_DEFAULT_HASH_CHARS CHAIN_DEFAULT_HASH_BYTES * 2

typedef struct block_t {
	unsigned long index;
	char *timestamp;
	char *currentHash;
	char *previousHash;
	char *data;
	int hash_length;
} block_t;


int chain_init();
int block_init(block_t *b, unsigned long index, char *timestamp, char *previousHash, char *data);
void block_get_index_str(block_t *b, char *buffer);
void block_get_time_str(block_t *b, char *buffer);
void block_compute_hash(block_t *b);
void block_set_time(block_t *b);
void block_print(block_t *b);
void block_free(block_t *b);
void block_flush(block_t *b, redisContext *ctx);

#endif
