//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_QUEUEIMPLEMENTATION_H
#define ESCAPEROOM_QUEUEIMPLEMENTATION_H

#include "objects.h"



#define SIZE 4

//int head, tail, currentSize;

//Group queue[SIZE];



/**
 * Initialise queue
 */
 /*
Group init(){
    head = 0;
    tail = 0;
    currentSize = 0;
}
  */

/**
 * Enqueue element to head
 * @param element
 * @return 0 if exception else 1
 */
 /*
int enqueue(Group element){
    if (isFull()) return 0;                     // overflow

    queue[tail] = element;
    tail = (tail + 1) % SIZE;                   // wrap cyclically
    currentSize++;

    return 1;
}
 */

/**
 * Dequeue element from tail
 * @return 0 if exception else dequeued element
 */
 /*
Group dequeue(){
    Group dequeuedElement;

    if (isEmpty()) {                           // underflow
        //dequeuedElement.id = (pthread_t) NULL;
        return dequeuedElement;
    }

    dequeuedElement = queue[head];
    head = (head + 1) % SIZE;                   // wrap cyclically
    currentSize--;

    return dequeuedElement;
}
*/
/**
 * Check if queue is empty
 * @return 1 if successful else 0
 */
 /*
int isEmpty(){
    return currentSize == 0;
}
*/
/**
 * Check if queue is full
 * @return 1 if successful else 0
 */
/*
int isFull(){
    return currentSize == SIZE;
}
*/
#endif //ESCAPEROOM_QUEUEIMPLEMENTATION_H