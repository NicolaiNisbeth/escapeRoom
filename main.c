
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

#define NUM_CHAIRS 1
#define NUM_GROUPS 2
#define NUM_INSTRUCTORS 1
#define GROUPS_PR_INSTRUCTOR 1

sem_t mutex_chairs;
sem_t mutex_groups;
sem_t sem_instructor;

int freeChairs = NUM_CHAIRS;
int availableGroups = 0;
Queue queue;
Instructor instructor[NUM_INSTRUCTORS];
Group group[NUM_GROUPS];

int main(int argc, char *argv[]) {
    queue = *initQueue(NUM_CHAIRS);

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

            Group inTheEscapeRoom[GROUPS_PR_INSTRUCTOR];
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++){
                freeChairs++;
                inTheEscapeRoom[i] = dequeue(&queue);
                printf("Instructor %s welcomes %c to the escape room\tFree chairs = %d/%d\n", ins->name, inTheEscapeRoom[i].name, freeChairs, NUM_CHAIRS);
                sem_post(&sem_instructor);  // instructor signals to group that he is ready
            }

            sem_post(&mutex_chairs);    // free mutex lock on chair count

            // time in escape room.
            ins->sleepTime = (rand() % 5) + 4;
            sleep(ins->sleepTime);

            printf("Instructor kicks group %c and group %c out of room\n", inTheEscapeRoom[0].name, inTheEscapeRoom[1].name);
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

        if (freeChairs > 0){
            freeChairs--;
            printf("Group %c takes a chair in waiting room.\t\t\tFree chairs = %d/%d\n", g->name, freeChairs, NUM_CHAIRS);
            sem_post(&mutex_chairs);    // free mutex lock on chair count

            sem_wait(&mutex_groups);
            availableGroups++;          // announcing group's arrival
            enqueue(&queue, *g);
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