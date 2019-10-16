//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_TYPE_H
#define ESCAPEROOM_TYPE_H

typedef struct {
    pthread_t id;
    char name;
    int inEscapeRoom;
} Group;

typedef struct {
    pthread_t id;
    char *name;
    int sleepTime;
} Instructor;

typedef struct {
    int *array;
    int size;
    int head;
    int tail;
    sem_t *mutex;
    sem_t *groups;
    sem_t *counter;
} Queue;

#endif //ESCAPEROOM_TYPE_H
