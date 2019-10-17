#pragma clang diagnostic ignored "-Wmissing-noreturn"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "queue.h"
#include "stringStore.h"

/** Conditions
 * NUM_CHAIRS   >=  GROUPS_PR_INSTRUCTOR
 * NUM_GROUPS   >=  GROUPS_PR_INSTRUCTOR
 *
 * NUM_INSTRUCTORS = 1
 */

#define NUM_CHAIRS 2
#define NUM_GROUPS 12
#define GROUPS_PR_INSTRUCTOR 2
#define ANSWERS_PR_GROUP 2
#define NUM_INSTRUCTORS 1

Queue chairs;
const int SOLUTION = 0;
int mystery = GROUPS_PR_INSTRUCTOR * ANSWERS_PR_GROUP;
Instructor instructors[NUM_INSTRUCTORS];
Group groups[NUM_GROUPS];
sem_t availableInstructor, mysteryKey, submission, mutex;

void *group_behaviour(void *param);
void *instructor_behaviour(void *param);
Instructor *getInstructor(pthread_t pID);
Group *getGroup(pthread_t pID);
void goodbye(char *status, Group *group);

int main(int argc, char *argv[]) {
    chairs = *init_queue(NUM_CHAIRS);

    // Initialize semaphores
    sem_init(&availableInstructor, 0, 0);
    sem_init(&mysteryKey, 0, 0);
    sem_init(&submission, 0, 0);
    sem_init(&mutex, 0, 1);

    // Create instructors' thread
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructors[i].name = &NAMES[0][i];
        pthread_create(&instructors[i].id, NULL, instructor_behaviour, NULL);
    }

    // Create groups' threads
    for (int i = 0, name = 'A'; i < NUM_GROUPS; i++, name++){
        groups[i].name = (char) name;
        sleep(rand() % 2);
        pthread_create(&groups[i].id, NULL, group_behaviour, NULL);
    }

    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(groups[i].id, NULL);

    exit(0);
}



void *instructor_behaviour(void *param) {
    Instructor *instructor = getInstructor(pthread_self());

    while(1) {
        sem_wait(&mutex);
        if (chairs.taken >= GROUPS_PR_INSTRUCTOR){
            Group inEscapeRoom[GROUPS_PR_INSTRUCTOR];
            sem_post(&mutex);

            // Invite n groups to escape room
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++){
                inEscapeRoom[i] = leave_chair(&chairs);
                printf(INSTRUCTOR_MSG, instructor->name, inEscapeRoom[i].name, chairs.taken, NUM_CHAIRS);
                sem_post(&availableInstructor);  // Becomes available to a group
            }

            // Release mystery
            sem_post(&mysteryKey);

            // await groups' submission
            while (submission.__align != GROUPS_PR_INSTRUCTOR);

            // say goodbye to groups
            (mystery == SOLUTION) ? goodbye(WINNER, inEscapeRoom) : goodbye(LOSER, inEscapeRoom);

            // Clean and prepare escape room for future groups
            sem_wait(&mysteryKey);
            mystery = ANSWERS_PR_GROUP * GROUPS_PR_INSTRUCTOR;
            sem_init(&submission, 0, 0);
        }
        else {
            sem_post(&mutex);
        }
    }
}

void *group_behaviour(void *param) {
    Group *group = getGroup(pthread_self());

    sem_wait(&mutex);
    if (chairs.taken < NUM_CHAIRS){
        take_chair(&chairs, *group);
        printf(WELCOME_MSG, group->name, chairs.taken, NUM_CHAIRS);
        sem_post(&mutex);

        // wait for available instructory
        sem_wait(&availableInstructor);

        // group in escape room, tries to solve mystery together with n other groups
        for (int i = 0, updated; i < ANSWERS_PR_GROUP; i++){
            sem_wait(&mysteryKey);
            updated = mystery - 1;
            printf(ANSWER_MSG, group->name, mystery, updated);
            mystery = updated;
            sem_post(&mysteryKey);
            sleep(2);
        }

        // submit solution to instructor
        sem_post(&submission);
        pthread_exit(&group->id);
    }
    else {
        sem_post(&mutex);
        printf(NO_FREE_CHAIR_MSG, group->name);
        pthread_exit(&group->id);
    }
}

Instructor *getInstructor(pthread_t pID) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i)
        if (instructors[i].id == pID)
            return &instructors[i];
}

Group *getGroup(pthread_t pID) {
    for (int i = 0; i < NUM_GROUPS; ++i)
        if (groups[i].id == pID)
            return &groups[i];
}

void goodbye(char *status, Group *group) {
    int statusIsWinner = strcmp(status, WINNER) == 0;

    if (statusIsWinner) printf("%s", CONGRATULATIONS_MSG);
    else printf("%s", UNLUCKY_MSG);

    for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++) printf(GROUP_NAME_MSG, group[i].name);

    if (statusIsWinner) printf("%s", MYSTERY_SOLVED_MSG);
    else printf("%s", MYSTERY_FAILED_MSG);
}