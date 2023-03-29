/*
 * Planificator threaduri
 *
 * 
 */

 #include <stdio.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <stdlib.h>
 #include <string.h>

 #include "so_scheduler.h"
 #include "queue.h"
 #include "struct.h"

scheduler_t *scheduler;

int so_init(unsigned int time_quantum, unsigned int io)
{
    // verifica daca planificatorul a fost deja initializat
    if (scheduler == NULL) {
        // se verifica parametrii
        if (time_quantum <= 0 || io > SO_MAX_NUM_EVENTS) {
            goto exit_init;
        }

        // se initializeaza planificatorul
        scheduler = malloc(sizeof(struct scheduler_t));
        if (scheduler == NULL) {
            perror("Malloc didn't work");
	    	goto exit_init;
        }

        scheduler->queue = q_create(sizeof(struct thread_t));
        scheduler->threads = q_create(sizeof(struct thread_t));
        scheduler->current_running = NULL;

        int err = sem_init(&(scheduler->running), 0, 1);
	    if (err == -1) {
            perror("Sem_init didn't work");
            goto exit_init;
        }

	    scheduler->max_quantum = time_quantum;
        scheduler->max_events = io;

	    return 0;
    }

exit_init:
    return -1;
}

void so_schedule(void)
{
    // nu exista niciun thread in starea "Ready"
    if (q_is_empty(scheduler->queue) == 1) {
        // threadul curent e in starea "Terminated"
		if (strcmp(scheduler->current_running->state, "Terminated") == 0) {
            // se opreste planificatorul
			int err = sem_post(&(scheduler->running));
            
            if (err == -1) {
                perror("Sem_post didn't work");
                goto exit_schedule;
            }
		}
		
        // threadul curent este singurul care mai poate fi rulat
		int err = sem_post(&(scheduler->current_running->sem));

        if (err == -1) {
            perror("Sem_post didn't work");
            goto exit_schedule;
        }
	} else {
        // se scoate din coada de prioritati primul thread in starea "Ready"
        ll_node_t *node = (ll_node_t *)q_peek(scheduler->queue);
        thread_t *thread = (thread_t *)(node->data);

        // nu exista niciun thread curent care sa ruleze
	    if (scheduler->current_running == NULL) {
            // threadul curent devine cel scos din coada de prioritati
	    	scheduler->current_running = thread;
            q_pop(scheduler->queue);

	        strcpy(thread->state, "Running");
            // cuanta de timp după care un proces trebuie preemptat
	        thread->quantum = scheduler->max_quantum;

	        int err = sem_post(&(thread->sem));

	        if (err == -1) {
                perror("Sem_post didn't work");
                goto exit_schedule;
            }

	    	goto exit_schedule;
	    }

        // threadul curent nu mai este in starea "Running"
	    if (strcmp(scheduler->current_running->state, "Running") != 0) {
            // threadul curent devine cel scos din coada de prioritati
	    	scheduler->current_running = thread;
            q_pop(scheduler->queue);

	        strcpy(thread->state, "Running");
            // cuanta de timp după care un proces trebuie preemptat
	        thread->quantum = scheduler->max_quantum;

	        int err = sem_post(&(thread->sem));

	        if (err == -1) {
                perror("Sem_post didn't work");
                goto exit_schedule;
            }

	    	goto exit_schedule;
	    }

        // threadul curent are o prioritate mai mica decat alte threaduri aflate in starea "Ready"
	    if (scheduler->current_running->priority < thread->priority) {
            // threadul curent este adaugat inapoi in coada de prioritati
            q_push_priority(scheduler->queue, scheduler->current_running, scheduler->current_running->priority);
            strcpy(scheduler->current_running->state, "Ready");

            // threadul curent devine cel scos din coada de prioritati
	    	scheduler->current_running = thread;
            q_pop(scheduler->queue);

	        strcpy(thread->state, "Running");
            // cuanta de timp după care un proces trebuie preemptat
	        thread->quantum = scheduler->max_quantum;

	        int err = sem_post(&(thread->sem));

	        if (err == -1) {
                perror("Sem_post didn't work");
                goto exit_schedule;
            }

	    	goto exit_schedule;
	    }

        // threadul curent trebuie preemptat
	    if (scheduler->current_running->quantum == 0) {
            // threadurile cu aceeasi prioritate sunt planificate după modelul Round Robin
	    	if (scheduler->current_running->priority == thread->priority) {
                // threadul curent este adaugat inapoi in coada de prioritati
                q_push_priority(scheduler->queue, scheduler->current_running, scheduler->current_running->priority);
	            strcpy(scheduler->current_running->state, "Ready");

                // threadul curent devine cel scos din coada de prioritati
	    		scheduler->current_running = thread;
                q_pop(scheduler->queue);

	            strcpy(thread->state, "Running");
                // cuanta de timp după care un proces trebuie preemptat
	            thread->quantum = scheduler->max_quantum;

	            int err = sem_post(&(thread->sem));

	            if (err == -1) {
                    perror("Sem_post didn't work");
                    goto exit_schedule;
                }

	    		goto exit_schedule;
	    	}
            // nu exista un alt thread in starea "Ready" cu prioritate mai mare sau egala
            // se replanifica threadul curent
	        scheduler->current_running->quantum = scheduler->max_quantum;
	    }

	    int err = sem_post(&(scheduler->current_running->sem));
        if (err == -1) {
            perror("Sem_post didn't work");
           goto exit_schedule;
        }
    }

exit_schedule:
    return;
}

void *start_thread(void *params)
{
	thread_t *thread = (thread_t *)params;

    // se asteapta planificatorul
	int err = sem_wait(&(thread->sem));

	if (err == -1) {
        perror("Sem_wait didn't work");
        goto exit_start;
    }

    // se ruleaza handlerul
	thread->handler(thread->priority);
    strcpy(thread->state, "Terminated");

    // se apeleaza planificatorul
	so_schedule();

exit_start:
	return NULL;
}

tid_t so_fork(so_handler *func, unsigned int priority)
{
    // se verifica parametrii
	if (func != NULL && priority <= SO_MAX_PRIO) {
        // se initializeaza un nou thread
	    thread_t *thread = malloc(sizeof(struct thread_t));

	    if (thread == NULL) {
            perror("Malloc didn't work");
	    	goto exit_fork;
        }

        thread->id = INVALID_TID;
        thread->handler = func;

        int err = sem_init(&(thread->sem), 0, 0);
	    if (err == -1) {
            perror("Sem_init didn't work");
            goto exit_fork;
        }

	    strcpy(thread->state, "New");
        thread->io = scheduler->max_events;
	    thread->quantum = scheduler->max_quantum;
	    thread->priority = priority;

	    err = pthread_create(&(thread->id), NULL, &start_thread, thread);
	    if (err != 0) {
            perror("Pthread_create didn't work");
            goto exit_fork;
        }

        // se adauga in coada cu toate threadurile
        q_push(scheduler->threads, thread, thread->priority);

        strcpy(thread->state, "Ready");
        // se adauga in coada de prioritati
        q_push_priority(scheduler->queue, thread, thread->priority);

        // in functie de threadul curent, se continua executia
        // sau se apeleaza planificatorul
	    if (scheduler->current_running == NULL)
	    	so_schedule();
	    else
	    	so_exec();

	    return thread->id;
    }

exit_fork:
    return INVALID_TID;
}

int so_wait(unsigned int io)
{
    // se verifica parametrul
	if (io < scheduler->max_events) {
        // threadul curent trece in starea "Waiting"
        strcpy(scheduler->current_running->state, "Waiting");
	    scheduler->current_running->io = io;

        // se continua executia
	    so_exec();
	    return 0;
    }

    return -1;
}

int so_signal(unsigned int io)
{
    // se verifica parametrul
	if (io < scheduler->max_events) {
        unsigned int number_ready = 0;

        // toate threadurile care asteapta evenimentul io trec
        // inapoi in starea "Ready"
	    for (int ind = 0; ind < q_get_size(scheduler->threads); ++ind) {
            ll_node_t *node = get_nth_node(scheduler->threads->list, ind);
            thread_t *thread = (thread_t *)(node->data);

		    if (thread->io == io && strcmp(thread->state, "Waiting") == 0) {
                thread->io = scheduler->max_events;
			    strcpy(thread->state, "Ready");

                // se adauga inapoi in coada de prioritati
                q_push_priority(scheduler->queue, thread, thread->priority);
			    ++number_ready;
            }
        }

        // se continua executia
	    so_exec();
	    return number_ready;
    }

    return -1;
}

void so_exec(void)
{
    // cuanta de timp a threadului curent este decrementata
	--(scheduler->current_running->quantum);

    thread_t *thread = scheduler->current_running;
    // se apeleaza planificatorul
	so_schedule();

    // threadul este preemptat
	int err = sem_wait(&(thread->sem));

	if (err == -1) {
        perror("Sem_wait didn't work");
        goto exit_exec;
    }

exit_exec:
    return;
}

void so_end(void)
{
    // se verifica daca planificatorul a fost initializat
	if (scheduler != NULL) {
        // se parcurge coada cu toate threadurile
	    for (int ind = 0; ind < q_get_size(scheduler->threads); ind++) {
            ll_node_t *node = get_nth_node(scheduler->threads->list, ind);
            thread_t *thread = (thread_t *)(node->data);

            // se asteapta ca threadul sa isi termine executia
		    int err = pthread_join(thread->id, NULL);

		    if (err != 0) {
                perror("Pthread_join didn't work");
                goto exit_end;
            }

            err = sem_destroy(&(thread->sem));
	        if (err == -1) {
                perror("Sem_destroy didn't work");
                goto exit_end;
            }
	    }

        // se elibereaza din memorie cozile
        q_free(scheduler->threads);
        q_free(scheduler->queue);

	    int err = sem_destroy(&(scheduler->running));

	    if (err == -1) {
            perror("Sem_destroy didn't work");
            goto exit_end;
        }

        // se elibereaza din memorie planificatorul
        free(scheduler);
        scheduler = NULL;
    }

exit_end:
    return;
}

