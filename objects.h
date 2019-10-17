//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_OBJECTS_H
#define ESCAPEROOM_OBJECTS_H

typedef struct {
    pthread_t id;
    char name;
} Group;

typedef struct {
    pthread_t id;
    char *name;
} Instructor;

typedef struct {
    Group *array;
    int size;
    int head;
    int tail;
    int taken;
    sem_t *mutex;
} Queue;

#endif //ESCAPEROOM_OBJECTS_H
