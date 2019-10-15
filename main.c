
//gcc -Wall -pthread
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
#define NUM_CHAIRS 4

int curChairs = 4;
struct Instructor instructor[1];
struct Group grp[MAX];

int main(int argc, char *argv[]) {


    printf("Main thread beginning\n");
    /* 1. Get command line arguments */
    int runTime,clients;
    struct Group group = init();

    runTime = 30;
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
        pthread_create(&grp[i].id, NULL, client, NULL);
        //enqueue(grp[i]);
        printf("Spawning group %c\n", grp[i].name);
    }

    /* 5. Sleep. */
    printf("Main thread sleeping for %d seconds\n", runTime);
    sleep(runTime);
    /* 6. Exit.  */
    printf("Main thread exiting\n");
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
        /* wait for a client to become available (sem_client) */
        sem_wait(&sem_client);
        /* wait for mutex to access chair count (chair_mutex) */
        sem_wait(&chairs_mutex);
        /* increment number of chairs available */
        curChairs += 1;
        //struct Group g = dequeue();
        //printf("Instructor %s welcoming %c. Number of chairs available = %d/%d\n", ins->name, g.name, curChairs, NUM_CHAIRS);
        printf("Instructor %s welcoming ___. Number of chairs available = %d/%d\n", ins->name, curChairs, NUM_CHAIRS);
        /* signal to client that barber is ready to cut their hair (sem_barber) */
        sem_post(&sem_barber);
        /* give up lock on chair count */
        sem_post(&chairs_mutex);
        /* generate random number, worktime, from 1-4 seconds for length of haircut.  */
        ins->sleepTime = (rand() % 4) + 1;
        /* cut hair for worktime seconds (really just call sleep()) */
        printf("Instructor instructing for %d seconds\n", ins->sleepTime);
        sleep(ins->sleepTime);
    }
}

struct Group *getGroup(pthread_t self) {
    for (int i = 0; i < NUM_GROUPS; ++i) {
        if (grp[i].id == self)
            return &grp[i];
    }
}

void *client(void *param) {
    int waittime;

    struct Group *g = getGroup(pthread_self());

    while(1) {
        /* wait for mutex to access chair count (chair_mutex) */
        sem_wait(&chairs_mutex);
        /* if there are no chairs */
        if(curChairs <= 0){
            /* free mutex lock on chair count */
            printf("Group %c leaving with no instruction\n", g->name);
            sem_post(&chairs_mutex);
        }
            /* else if there are chairs */
        else{
            /* decrement number of chairs available */
            curChairs -= 1;
            printf("Group %c goes to waiting room. Number of chairs = %d/%d\n", g->name, curChairs, NUM_CHAIRS);
            /* signal that a customer is ready (sem_client) */
            sem_post(&sem_client);
            /* free mutex lock on chair count */
            sem_post(&chairs_mutex);
            /* wait for barber (sem_barber) */
            sem_wait(&sem_barber);
            /* get haircut */
            printf("Group %c getting instructed\n",g->name);
        }
        /* generate random number, waittime, for length of wait until next haircut or next try.  Max value from command line. */
        waittime = (rand() % clientWait) + 1;
        /* sleep for waittime seconds */
        printf("Group %c waiting %d seconds before reaching out to instructor\n", g->name, waittime);
        sleep(waittime);
    }
}


