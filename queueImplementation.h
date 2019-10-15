//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_QUEUEIMPLEMENTATION_H
#define ESCAPEROOM_QUEUEIMPLEMENTATION_H

#include "type.h"



#define SIZE 4

int head, tail, currentSize;

struct Group queue[SIZE];

struct Group init();
int enqueue(struct Group element);
struct Group dequeue();
int isEmpty();
int isFull();

/**
 * Initialise queue
 */
struct Group init(){
    head = 0;
    tail = 0;
    currentSize = 0;
}

/**
 * Enqueue element to head
 * @param element
 * @return 0 if exception else 1
 */
int enqueue(struct Group element){
    if (isFull()) return 0;                     // overflow

    queue[tail] = element;
    tail = (tail + 1) % SIZE;                   // wrap cyclically
    currentSize++;

    return 1;
}

/**
 * Dequeue element from tail
 * @return 0 if exception else dequeued element
 */
struct Group dequeue(){
    struct Group dequeuedElement;

    if (isEmpty()) {                           // underflow
        dequeuedElement.id = (pthread_t) NULL;
        return dequeuedElement;
    }

    dequeuedElement = queue[head];
    head = (head + 1) % SIZE;                   // wrap cyclically
    currentSize--;

    return dequeuedElement;
}

/**
 * Check if queue is empty
 * @return 1 if successful else 0
 */
int isEmpty(){
    return currentSize == 0;
}

/**
 * Check if queue is full
 * @return 1 if successful else 0
 */
int isFull(){
    return currentSize == SIZE;
}

#endif //ESCAPEROOM_QUEUEIMPLEMENTATION_H