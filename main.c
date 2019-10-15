
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

sem_t chairs_mutex;
sem_t sem_client;
sem_t sem_barber;
int clientWait;

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

    clientWait = 2;

    /* 2. Initialize semaphores */
    sem_init(&chairs_mutex,0,1);
    sem_init(&sem_client,0,0);
    sem_init(&sem_barber,0,0);

    /* 3. Create instructor thread. */
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructor[i].name = "Linda";
        pthread_create(&instructor[i].id, NULL, barber, NULL);
        printf("Spawning instructor %s\n", instructor[i].name);
    }

    /* 4. Create group threads.  */
    for (int i = 0, j = 'A'; i < NUM_GROUPS; i++, j++){
        grp[i].name = (char) j;
        //sleep(1);
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

        sem_wait(&chairs_mutex);    // wait for mutex to access chair count
        freeChairs++;
        sem_post(&chairs_mutex);    // free mutex lock on chair count

        // wait for group to become available
        sem_wait(&sem_client);

        //struct Group g = dequeue();
        //printf("Instructor %s welcoming %c. Number of chairs available = %d/%d\n", ins->name, g.name, freeChairs, NUM_CHAIRS);
        printf("Instructor %s welcoming ___.\t\t\t\t\tFree chairs = %d/%d\n", ins->name, freeChairs, NUM_CHAIRS);


        /* signal to client that barber is ready to cut their hair (sem_barber) */
        sem_post(&sem_barber);
        /* give up lock on chair count */
        /* generate random number, worktime, from 1-4 seconds for length of haircut.  */
        ins->sleepTime = (rand() % 4) + 1;
        /* cut hair for worktime seconds (really just call sleep()) */
        sleep(ins->sleepTime);
        printf("Instructor kicks ___ out of room\n");
        //printf("Instructor instructing for %d seconds\n", ins->sleepTime);
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
        sem_wait(&chairs_mutex);        // wait for mutex to access chair count

        if (freeChairs){
            freeChairs--;
            sem_post(&chairs_mutex);    // free mutex lock on chair count

            printf("Group %c takes a chair in waiting room.\t\t\tFree chairs = %d/%d\n", g->name, freeChairs, NUM_CHAIRS);

            // increment semaphore to show that group is ready
            sem_post(&sem_client);

            // wait for instructor
            sem_wait(&sem_barber);

            printf("Group %c is taken to escape room\n",g->name);

            pthread_exit(&g->id);
        }
        else {
            // free mutex lock on chair count
            printf("Group %c is leaving without instruction\n", g->name);
            sem_post(&chairs_mutex);
            pthread_exit(&g->id);
        }

    }
}



#pragma clang diagnostic pop