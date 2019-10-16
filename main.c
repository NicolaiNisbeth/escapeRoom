
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"

void *client(void *param);
void *barber(void *param);
Instructor * getInstructor(pthread_t pID);
Group * getGroup(pthread_t pID);

#define NUM_GROUPS 12
#define NUM_INSTRUCTORS 1
#define NUM_CHAIRS 6
#define GROUPS_PR_INSTRUCTOR 2

sem_t mutex_chairs;
sem_t mutex_groups;
sem_t sem_instructor;

int freeChairs = NUM_CHAIRS;
int availableGroups = 0;
Instructor instructor[NUM_INSTRUCTORS];
Group group[NUM_GROUPS];

int main(int argc, char *argv[]) {
    Queue queue = *initQueue(NUM_CHAIRS);

    // Initialize semaphores
    sem_init(&mutex_chairs,0,1);
    sem_init(&mutex_groups,0,1);
    sem_init(&sem_instructor,0,0);

    // Create instructor thread
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructor[i].name = "Linda";
        pthread_create(&instructor[i].id, NULL, barber, NULL);
        //printf("Spawning instructor %s\n", instructor[i].name);
    }

    // Create group threads
    for (int i = 0, name = 'A'; i < NUM_GROUPS; i++, name++){
        group[i].name = (char) name;
        sleep(rand() % 4);
        pthread_create(&group[i].id, NULL, client, NULL);
        //enqueue(&queue, group[i].id);

        //printf("Spawning group %c\n", group[i].name);
    }

    //for (int i = 0; i < NUM_INSTRUCTORS; i++) pthread_join(instructor[i].id, NULL);
    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(group[i].id, NULL);

    exit(0);
}



void *barber(void *param) {
    Instructor *ins = getInstructor(pthread_self());

    while(1) {
        sem_wait(&mutex_groups);

        if (availableGroups >= GROUPS_PR_INSTRUCTOR){
            availableGroups -= GROUPS_PR_INSTRUCTOR;
            sem_post(&mutex_groups);


            sem_wait(&mutex_chairs);    // wait for mutex to access chair count
            freeChairs += GROUPS_PR_INSTRUCTOR;

            printf("Instructor %s takes group ___ & ___\t\t\tFree chairs = %d/%d\n", ins->name, freeChairs, NUM_CHAIRS);
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++){
                sem_post(&sem_instructor);  // instructor signals to group that he is ready
            }

            sem_post(&mutex_chairs);    // free mutex lock on chair count

            // time in escape room.
            ins->sleepTime = (rand() % 5) + 4;
            sleep(ins->sleepTime);

            printf("Instructor kicks ___ & ___ out of room\n");
        }
        else {
            sem_post(&mutex_groups);
        }
    }
}

void *client(void *param) {
    Group *g = getGroup(pthread_self());

    while(1) {
        sem_wait(&mutex_chairs);        // wait for mutex to access chair count

        if (freeChairs){
            freeChairs--;
            printf("Group %c takes a chair in waiting room.\t\t\tFree chairs = %d/%d\n", g->name, freeChairs, NUM_CHAIRS);
            sem_post(&mutex_chairs);    // free mutex lock on chair count

            sem_wait(&mutex_groups);
            availableGroups++;          // announcing its arrival
            sem_post(&mutex_groups);

            sem_wait(&sem_instructor);  // wait for instructor

            printf("Group %c is in the escape room\n",g->name);

            // action


            pthread_exit(&g->id);
        }
        else {
            // free mutex lock on chair count
            printf("Group %c is leaving without instruction\n", g->name);
            sem_post(&mutex_chairs);
            pthread_exit(&g->id);
        }

    }
}

Instructor * getInstructor(pthread_t pID) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i) if (instructor[i].id == pID){
            return &instructor[i];
        }

}

Group * getGroup(pthread_t pID) {
    for (int i = 0; i < NUM_GROUPS; ++i) if (group[i].id == pID){
            return &group[i];
        }
}

#pragma clang diagnostic pop