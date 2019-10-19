#ifndef ESCAPEROOM_QUEUE_H
#define ESCAPEROOM_QUEUE_H
#include "objects.h"
#include "stringstore.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int queue_incr(Queue *queue, int val);
void take_chair(Queue *queue, Group group);
Group leave_chair(Queue *queue);
int full(Queue *queue);
int empty(Queue *queue);
sem_t* createSemaphore(int value);

/**
 * Initialise queue
 * @param size of queue
 * @return queue pointer to initialised queue
 */
Queue* setup_chairs(int size){
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->size = size;
    queue->array = (Group *) malloc(size * sizeof(Group));
    queue->head = 0;
    queue->tail = 0;
    queue->taken = 0;
    queue->mutex = createSemaphore(1);
    return queue;
}

/**
 * Enqueue group to tail in queue
 * @param queue
 * @param group
 */
void take_chair(Queue *queue, Group group){
    sem_wait(queue->mutex);

    if (full(queue)){
        sem_post(queue->mutex);
        perror(FULL_QUEUE_MSG), exit(1);
    }
    else {
        queue->array[queue->tail] = group;
        queue->tail = queue_incr(queue, queue->tail);
        queue->taken++;

        sem_post(queue->mutex);
    }
}

/**
 * Dequeue group from head
 * @param queue
 * @return dequeued group
 */
Group leave_chair(Queue *queue){
    sem_wait(queue->mutex);

    if (empty(queue)){
        sem_post(queue->mutex);
        perror(EMPTY_QUEUE_MSG), exit(1);
    }
    else {
        Group group = queue->array[queue->head];
        queue->head = queue_incr(queue, queue->head);
        queue->taken--;
        sem_post(queue->mutex);

        return group;
    }
}

/**
 * Increment current value and return updated value, cyclically
 * @param queue
 * @param val current value
 * @return updated value
 */
int queue_incr(Queue *queue, int val) {
    return (val + 1) % queue->size;
}

/**
 * Check if queue is full
 * @return 1 if full else 0
 */
int full(Queue *queue){
    return queue->taken == queue->size;
}

/**
 * Check if queue is empty
 * @return 1 if empty else 0
 */
int empty(Queue *queue){
    return queue->taken == 0;
}

/**
 * Creates a semaphore by allocating memory and initializing it
 * @param value
 * @return pointer to initialised semaphore
 */
sem_t* createSemaphore(int value){
    sem_t *sem = malloc(sizeof(sem_t));
    sem_init(sem, 0, value);
    return sem;
}

#endif //ESCAPEROOM_QUEUE_H
