#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chain.h"
#include "chain_redis.h"

int chain_redis_server_init(chain_redis_server_t *server) {
    char buffer[4096];
    FILE *process;

    if ((process = popen("redis-server", "r")) == NULL) {
        perror("Could not open redis server!");
        exit(CHAIN_RESULT_ERROR);
    }

    bool ready = false;
    printf("Waiting for redis-server to initialize.");

    while(fgets(buffer, sizeof(buffer), process)) {
        if (strstr(buffer, "Address already in use") != NULL) {
            printf("\nAddress already in use! Please close your redis server down");
            printf("and try again.\n");
            exit(CHAIN_RESULT_ERROR);
        } else if (strstr(buffer, "Ready to accept connections") != NULL) {
            ready = true;
            break;
        } else {
            printf(".");
        }
    }

    if (!ready) {
        printf("Could not start redis-server for unknown reason.\n");
        printf("Please run 'redis-server' in a command prompt and check the error message.\n");
        exit(CHAIN_RESULT_ERROR);
    }

    printf("done!\n");

    server->process = process;
    server->ready = true;

    return CHAIN_RESULT_OK;
}

void chain_redis_server_close(chain_redis_server_t *server) {
    if (server->process != NULL) {
        // TODO: I acknowledge this is sloppy, need to make it better but
        // can't be bothered at the moment. Probably needs a solution with
        // forking instead.
        popen("ps -ax | awk '$4 ~ /redis-server/ {print $1}' | xargs -n1 kill", "r");
        pclose(server->process);
    }

    server->process = NULL;
    server->ready = false;
}