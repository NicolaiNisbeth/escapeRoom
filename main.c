
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "queueImplementation.h"

void *client(void *param);
void *barber(void *param);
struct Instructor *getInstructor(pthread_t pID);
struct Group *getGroup(pthread_t pID);

sem_t mutex_chairs;
sem_t sem_group;
sem_t sem_instructor;

#define NUM_GROUPS 4
#define NUM_INSTRUCTORS 1
#define NUM_CHAIRS 2

int freeChairs = 2;
struct Instructor instructor[NUM_INSTRUCTORS];
struct Group group[NUM_GROUPS];

int main(int argc, char *argv[]) {

    // Initialize semaphores
    sem_init(&mutex_chairs, 0, 1);
    sem_init(&sem_group, 0, 0);
    sem_init(&sem_instructor, 0, 0);

    // Create instructor thread
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructor[i].name = "Linda";
        pthread_create(&instructor[i].id, NULL, barber, NULL);
        printf("Spawning instructor %s\n", instructor[i].name);
    }

    // Create group threads
    for (int i = 0, letter = 'A'; i < NUM_GROUPS; i++, letter++){
        group[i].name = (char) letter;
        pthread_create(&group[i].id, NULL, client, NULL);
        //enqueue(group[i]);
        printf("Spawning group %c\n", group[i].name);
    }

    //for (int i = 0; i < NUM_INSTRUCTORS; i++) pthread_join(instructor[i].id, NULL);
    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(group[i].id, NULL);

    exit(0);
}



void *barber(void *param) {
    struct Instructor *ins = getInstructor(pthread_self());
    //struct Group g = dequeue();

    while(1) {
        sem_wait(&sem_group);      // wait for group to become available

        sem_wait(&mutex_chairs);    // wait for mutex to access chair count
        freeChairs++;

        //struct Group g = dequeue();
        //printf("Instructor %s welcoming %c. Number of chairs available = %d/%d\n", ins->name, g.name, freeChairs, NUM_CHAIRS);
        printf("Instructor %s welcomes ___.\t\t\t\t\tFree chairs = %d/%d\n", ins->name, freeChairs, NUM_CHAIRS);

        sem_post(&sem_instructor);  // instructor signals to group that he is ready
        sem_post(&mutex_chairs);    // free mutex lock on chair count

        // random number from 1-4 seconds for length of escape room.
        ins->sleepTime = (rand() % 4) + 1;
        sleep(ins->sleepTime);
        printf("Instructor kicks ___ out of room\n");
    }
}



void *client(void *param) {
    struct Group *g = getGroup(pthread_self());

    while(1) {
        sem_wait(&mutex_chairs);        // wait for mutex to access chair count

        if (freeChairs){
            freeChairs--;

            printf("Group %c takes a chair in waiting room.\t\t\tFree chairs = %d/%d\n", g->name, freeChairs, NUM_CHAIRS);

            sem_post(&sem_group);       // increment semaphore to show that group is ready
            sem_post(&mutex_chairs);    // free mutex lock on chair count

            sem_wait(&sem_instructor);  // wait for instructor

            printf("Group %c is taken to escape room\n",g->name);

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

struct Instructor *getInstructor(pthread_t pID) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i) {
        if (instructor[i].id == pID)
            return &instructor[i];
    }
}

struct Group *getGroup(pthread_t pID) {
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (group[i].id == pID)
            return &group[i];
    }
}

#pragma clang diagnostic pop