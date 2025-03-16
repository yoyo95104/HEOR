#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "utl.h"

#define MAX_THREADS 12

pthread_t thread_pool[MAX_THREADS];
AttackData *thread_data[MAX_THREADS];
int active_threads = 0;

void *thread_function(void *arg) {
    send_attack(arg);
    return NULL;
}

void start_thread_pool(AttackData *attack_data) {    for (int i = 0; i < MAX_THREADS; i++) {
        if (active_threads < MAX_THREADS) {
            thread_data[i] = malloc(sizeof(AttackData));
            memcpy(thread_data[i], attack_data, sizeof(AttackData));
		pthread_create(&thread_pool[i], NULL, thread_function, thread_data[i]);
            pthread_detach(thread_pool[i]);
            active_threads++;
        }
    }
}
