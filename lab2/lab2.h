#include "pthread.h"
#include "stdbool.h"

#define ERROR_SPAWN_THREAD -1

struct event
{
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    bool condition_is_fullilled;
    char *payload;
};

struct event default_event();

void *produce(void *args);

void *consume(void *args);

void spawn_thread_or_die(pthread_t *thread, const char thread_name[], void *callable, void *args);
