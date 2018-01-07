#ifndef CHAIN_REDIS_H
#define CHAIN_REDIS_H

#include <stdio.h>
#include <stdbool.h>

typedef struct chain_redis_server_t {
    bool ready;
    FILE *process;
} chain_redis_server_t;

int chain_redis_server_init(chain_redis_server_t *server);
void chain_redis_server_close(chain_redis_server_t *server);

#endif