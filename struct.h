#ifndef STRUCT_H_
#define STRUCT_H_

#include <pthread.h>
#include <semaphore.h>
#include "so_scheduler.h"
#include "queue.h"

// structura unui thread
typedef struct thread_t {
	tid_t id;
    so_handler *handler;
    sem_t sem;

	char state[11];
    unsigned int io;
	unsigned int quantum;
	unsigned int priority;
} thread_t;


// structura planificatorului
typedef struct scheduler_t {
    // coada de prioritati pentru threadurile
    // in starea "Ready"
    queue_t *queue;
    // coada cu toate threadurile
    queue_t *threads;
    thread_t *current_running;
    sem_t running;

    unsigned int max_quantum;
	unsigned int max_events;
} scheduler_t;

#endif