//
// Created by nicolainisbeth on 16/10/2019.
//

#ifndef ESCAPEROOM_QUEUE_H
#define ESCAPEROOM_QUEUE_H
#include "type.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

sem_t mutex_queue;
sem_t sem_groupsSS;
sem_t sem_counter;

int queue_incr(Queue *queue, int tail);

/**
 * Initialise queue
 */
Queue *initQueue(int size){
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->size = size;
    queue->array = (int *) malloc(size * sizeof(int));
    queue->head = 0;
    queue->tail = 0;
    queue->mutex = sem_init(&mutex_queue, 0, 1);
    queue->groups = sem_init(&sem_groupsSS, 0, 0);
    queue->counter = sem_init(&sem_counter, 0, (size-1));
    return queue;
}

/**
 * Enqueue element to head
 * @param queue
 * @param element
 */
void enqueue(Queue *queue, int element){
    sem_wait(queue->counter);
    sem_wait(queue->mutex);
    queue->array[queue->tail] = element;
    queue->tail = queue_incr(queue, queue->tail);

    sem_post(queue->mutex);
    sem_post(queue->groups);
}

/**
 * Dequeue element from tail
 * @param queue
 * @return dequeued element
 */
int dequeue(Queue *queue){
    sem_wait(queue->groups);
    sem_wait(queue->mutex);

    int element = queue->array[queue->head];
    queue->head = queue_incr(queue, queue->head);

    sem_post(queue->mutex);
    sem_post(queue->counter);

    return element;
}

int queue_incr(Queue *queue, int i) {
    return (i+1) % queue->size;
}

#endif //ESCAPEROOM_QUEUE_H
