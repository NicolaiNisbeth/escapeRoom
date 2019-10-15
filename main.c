
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"


#define MAX 20
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "queueImplementation.h"

void *client(void *param);
void *barber(void *param);
struct Instructor *getInstructor(pthread_t self);
struct Group *getGroup(pthread_t self);

sem_t mutex_chairs;
sem_t sem_group;
sem_t sem_instructor;

#define NUM_GROUPS 4
#define NUM_INSTRUCTORS 1
#define NUM_CHAIRS 2

int freeChairs = 2;
struct Instructor instructor[1];
struct Group grp[MAX];

int main(int argc, char *argv[]) {


    printf("Main thread beginning\n");
    /* 1. Get command line arguments */
    struct Group group = init();

    /* 2. Initialize semaphores */
    sem_init(&mutex_chairs, 0, 1);
    sem_init(&sem_group, 0, 0);
    sem_init(&sem_instructor, 0, 0);

    /* 3. Create instructor thread. */
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructor[i].name = "Linda";
        pthread_create(&instructor[i].id, NULL, barber, NULL);
        printf("Spawning instructor %s\n", instructor[i].name);
    }

    /* 4. Create group threads.  */
    for (int i = 0, letter = 'A'; i < NUM_GROUPS; i++, letter++){
        grp[i].name = (char) letter;
        //sleep(rand() % 4 + 1);
        pthread_create(&grp[i].id, NULL, client, NULL);
        //enqueue(grp[i]);
        printf("Spawning group %c\n", grp[i].name);
    }

    //for (int i = 0; i < NUM_INSTRUCTORS; i++) pthread_join(instructor[i].id, NULL);
    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(grp[i].id, NULL);





    /* 5. Sleep. */
    //printf("Main thread sleeping for %d seconds\n", runTime);
    //sleep(runTime);
    /* 6. Exit.  */
    //printf("Main thread exiting\n");
    exit(0);
}

struct Instructor *getInstructor(pthread_t self) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i) {
        if (instructor[i].id == self)
            return &instructor[i];
    }
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

struct Group *getGroup(pthread_t self) {
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (grp[i].id == self)
            return &grp[i];
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
            sem_post(&mutex_chairs);
            printf("Group %c is leaving without instruction\n", g->name);
            pthread_exit(&g->id);
        }

    }
}



#pragma clang diagnostic pop