//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_OBJECTS_H
#define ESCAPEROOM_OBJECTS_H

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
    Group *array;
    int size;
    int head;
    int tail;
    int counter;
    sem_t *mutex;
    sem_t *groups;
} Queue;

#endif //ESCAPEROOM_OBJECTS_H
