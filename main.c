
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"

#define NUM_CHAIRS 4
#define NUM_GROUPS 2
#define NUM_INSTRUCTORS 1
#define GROUPS_PR_INSTRUCTOR 1

void *group(void *param);
void *instructor(void *param);
Instructor * getInstructor(pthread_t pID);
Group * getGroup(pthread_t pID);

sem_t mutex_groups;
sem_t sem_instructor;

Queue queue;
Instructor instructors[NUM_INSTRUCTORS];
Group groups[NUM_GROUPS];

int main(int argc, char *argv[]) {
    queue = *initQueue(NUM_CHAIRS);

    // Initialize semaphores
    sem_init(&mutex_groups,0,1);
    sem_init(&sem_instructor,0,0);

    // Create instructors thread
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructors[i].name = "Linda";
        pthread_create(&instructors[i].id, NULL, instructor, NULL);
        //printf("Spawning instructors %s\n", instructors[i].name);
    }

    // Create groups threads
    for (int i = 0, name = 'A'; i < NUM_GROUPS; i++, name++){
        groups[i].name = (char) name;
        sleep(rand() % 10 + 6) ;
        pthread_create(&groups[i].id, NULL, group, NULL);
        //printf("Spawning groups %c\n", groups[i].name);
    }

    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(groups[i].id, NULL);
    for (int i = 0; i < NUM_INSTRUCTORS; i++) pthread_join(instructors[i].id, NULL);

    exit(0);
}



void *instructor(void *param) {
    Instructor *ins = getInstructor(pthread_self());

    while(1) {
        sem_wait(&mutex_groups);
        if (queue.counter >= GROUPS_PR_INSTRUCTOR){
            sem_post(&mutex_groups);

            Group inEscapeRoom[GROUPS_PR_INSTRUCTOR];
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++){
                inEscapeRoom[i] = dequeue(&queue);
                printf("Instructor %s welcomes %c to the escape room\t\tTaken chairs = %d/%d\n", ins->name, inEscapeRoom[i].name, queue.counter, NUM_CHAIRS);
                sem_post(&sem_instructor);  // instructors signals to groups that he is ready
            }

            // time in escape room.
            ins->sleepTime = (rand() % 5) + 4;
            sleep(ins->sleepTime);

            printf("Instructor is done with ");
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++) printf("group %c ", inEscapeRoom[i].name);
            puts("");


            // TODO: bug, find out when it is possible to exit
            if (isEmpty(&queue))
                printf("Instructor %s is done for the day\n", ins->name), pthread_exit(&ins->id);
        }
        else {
            sem_post(&mutex_groups);
        }

    }
}

void *group(void *param) {
    Group *g = getGroup(pthread_self());

    while(1) {

        if (queue.counter < NUM_CHAIRS){

            sem_wait(&mutex_groups);
            enqueue(&queue, *g);
            printf("Group %c takes a chair in waiting room.\t\t\t\tTaken chairs = %d/%d\n", g->name, queue.counter, NUM_CHAIRS);
            sem_post(&mutex_groups);

            sem_wait(&sem_instructor);  // wait for instructors

            printf("Group %c is in the escape room\n",g->name);

            // action


            pthread_exit(&g->id);
        }
        else {
            printf("Group %c is leaving without instruction\n", g->name);
            pthread_exit(&g->id);
        }

    }
}

Instructor * getInstructor(pthread_t pID) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i) if (instructors[i].id == pID){
            return &instructors[i];
        }
}

Group * getGroup(pthread_t pID) {
    for (int i = 0; i < NUM_GROUPS; ++i) if (groups[i].id == pID){
            return &groups[i];
        }
}

#pragma clang diagnostic pop