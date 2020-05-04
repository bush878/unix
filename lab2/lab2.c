#include "stdio.h"
#include "lab2.h"
#include <unistd.h>
#include <stdlib.h>
#include "pthread.h"
#include "stdbool.h"

struct event default_event()
{
    struct event _event = {PTHREAD_MUTEX_INITIALIZER,
                           PTHREAD_COND_INITIALIZER,
                           false,
                           ""};
    return _event;
}

void *produce(void *args)
{
    struct event *subscribtion = (struct event *)args;

    char *possible_payloads[3] = {"Payload1", "Payload2", "Payload3"};
    int current_payload_id = 0;

    int error;

    while (true)
    {
        error = pthread_mutex_lock(&subscribtion->mutex);
        if (error)
        {
            break;
        }

        if (!subscribtion->condition_is_fullilled)
        {
            subscribtion->payload = possible_payloads[current_payload_id];
            current_payload_id = current_payload_id + 1;
            printf("Sending notification to consumer. Payload: \"%s\".\n", subscribtion->payload);
            subscribtion->condition_is_fullilled = true;

            error = pthread_cond_signal(&subscribtion->condition);
            if (error)
            {
                break;
            }
        }

        error = pthread_mutex_unlock(&subscribtion->mutex);
        if (error)
        {
            break;
        }

        if (current_payload_id == 3) {
            break;
        }

        sleep(1);
    }

    if (error)
    {
        pthread_exit((void *)EXIT_FAILURE);
    }
    pthread_exit((void *)EXIT_SUCCESS);
}

void *consume(void *args)
{
    struct event *subscribtion = (struct event *)args;

    int error;

    while (true)
    {
        error = pthread_mutex_lock(&subscribtion->mutex);
        if (error)
        {
            break;
        }

        while (!subscribtion->condition_is_fullilled)
        {
            error = pthread_cond_wait(&subscribtion->condition, &subscribtion->mutex);
            if (error)
            {
                break;
            }
        }
        printf("Got notification from producer. Payload: \"%s\".\n", subscribtion->payload);
        subscribtion->condition_is_fullilled = false;

        error = pthread_mutex_unlock(&subscribtion->mutex);
        if (error)
        {
            break;
        }
    }
    if (error)
    {
        pthread_exit((void *)EXIT_FAILURE);
    }
    pthread_exit((void *)EXIT_SUCCESS);
}

void spawn_thread_or_die(pthread_t *thread, const char thread_name[], void *callable, void *args)
{
    int error = pthread_create(thread, NULL, callable, args);
    if (error) {
        exit(-1);
    }
}

int main()
{
    struct event monitoredEvent = default_event();

    pthread_t consumer_thread;
    pthread_t producer_thread;

    const char consumer_name[] = "Consumer";
    const char producer_name[] = "Producer";

    spawn_thread_or_die(&producer_thread, producer_name, &produce, &monitoredEvent);
    spawn_thread_or_die(&consumer_thread, consumer_name, &consume, &monitoredEvent);

    int join_status;
    pthread_join(consumer_thread, (void *)&join_status);
    printf("%s stopped; Status %d\n", consumer_name, join_status);
    pthread_join(producer_thread, (void *)&join_status);
    printf("%s stopped; Status %d\n", producer_name, join_status);

    return 0;
}
