//
// Created by nicolainisbeth on 16/10/2019.
//

#ifndef ESCAPEROOM_QUEUE_H
#define ESCAPEROOM_QUEUE_H
#include "objects.h"
#include "queueImplementation.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>


int queueIncr(Queue *queue, int i);
sem_t *makeSemaphore(int value);
void enqueue(Queue *queue, Group element);
Group dequeue(Queue *queue);
int isFull(Queue *queue);
int isEmpty(Queue *queue);

/**
 * Initialise queue
 */
Queue *initQueue(int size){
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    queue->size = size;
    queue->array = (Group *) malloc(size * sizeof(Group));
    queue->head = 0;
    queue->tail = 0;
    queue->counter = 0;
    queue->mutex = makeSemaphore(1);
    queue->groups = makeSemaphore(0);
    return queue;
}


/**
 * Enqueue element to head
 * @param queue
 * @param element
 */
void enqueue(Queue *queue, Group element){
    //sem_wait(queue->counter);
    sem_wait(queue->mutex);

    if (isFull(queue)){
        sem_post(queue->mutex);
        perror("queue is full!");
    }
    else {
        queue->array[queue->tail] = element;
        queue->tail = queueIncr(queue, queue->tail);
        queue->counter++;

        sem_post(queue->mutex);
        //sem_post(queue->groups);
    }
}


/**
 * Dequeue element from tail
 * @param queue
 * @return dequeued element
 */
Group dequeue(Queue *queue){
    //sem_wait(queue->groups);
    sem_wait(queue->mutex);

    if (isEmpty(queue)){
        sem_post(queue->mutex);
        perror("queue is empty!");
    }
    else {
        Group element = queue->array[queue->head];
        queue->head = queueIncr(queue, queue->head);
        queue->counter--;
        sem_post(queue->mutex);
        //sem_post(queue->counter);

        return element;
    }

}

sem_t *makeSemaphore(int value){
    sem_t *sem = malloc(sizeof(sem_t));
    sem_init(sem, 0, value);
    return sem;
}

int queueIncr(Queue *queue, int i) {
    return (i+1) % queue->size;
}

/**
 * Check if queue is full
 * @return 1 if successful else 0
 */
int isFull(Queue *queue){
    return queue->counter == queue->size;
}

/**
 * Check if queue is empty
 * @return 1 if successful else 0
 */
int isEmpty(Queue *queue){
    return queue->counter == 0;
}

#endif //ESCAPEROOM_QUEUE_H
