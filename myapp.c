#pragma clang diagnostic ignored "-Wmissing-noreturn"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "queue.h"
#include "stringstore.h"

/** Conditions
 * NUM_CHAIRS   >=  GROUPS_PR_INSTRUCTOR
 * NUM_GROUPS   >=  GROUPS_PR_INSTRUCTOR
 * ANSWERS_PR_GROUP > 0
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
sem_t availableInstructor, mysteryKey, submission, chairsKey;

void *group_behaviour();
void *instructor_behaviour();
Instructor *getInstructor(pthread_t pID);
Group *getGroup(pthread_t pID);
void goodbye(char *status, Group *group);

/**
 * The Escape Room!
 * Chairs are set up, semaphores are initialised to secure mutual exclusion
 * among threads.
 * One instructor thread is given a name and a behaviour.
 * N Groups arrive periodically, each is assigned a name and a behaviour.
 * The instructor closes when the last group leaves the escape room.
 *
 * @return exit code
 */
int main(void) {
    // chairs are implemented as a queue, FIFO
    chairs = *setup_chairs(NUM_CHAIRS);

    // initialize semaphores
    sem_init(&availableInstructor, 0, 0);
    sem_init(&mysteryKey, 0, 0);
    sem_init(&submission, 0, 0);
    sem_init(&chairsKey, 0, 1);

    // create instructor thread
    for (int i = 0; i < NUM_INSTRUCTORS; i++){
        instructors[i].name = &NAMES[0][i];
        pthread_create(&instructors[i].id, NULL, instructor_behaviour, NULL);
    }

    // create group threads
    for (int i = 0, name = 'A'; i < NUM_GROUPS; i++, name++){
        groups[i].name = (char) name;
        pthread_create(&groups[i].id, NULL, group_behaviour, NULL);
        sleep(rand() % 2);
    }

    // close down after last group leaves the escape room
    for (int i = 0; i < NUM_GROUPS; i++) pthread_join(groups[i].id, NULL);

    exit(0);
}

/**
 * Group arrives and takes a chair if available else leaves!
 * Groups with chair await instructor's invitation to escape room
 * In escape room the group has a fixed number of answers. For
 * each answer the group decrements the shared mystery. The group
 * finally submits the solution to the instructor before exiting
 */
void *group_behaviour() {
    Group *group = getGroup(pthread_self());

    // wait for access to shared variable, chairs.taken
    sem_wait(&chairsKey);
    if (chairs.taken < NUM_CHAIRS){
        take_chair(&chairs, *group);
        printf(WELCOME_MSG, group->name, chairs.taken, NUM_CHAIRS);
        sem_post(&chairsKey);

        // wait for available instructor
        sem_wait(&availableInstructor);

        // group in escape room, tries to solve mystery together with n other groups
        for (int i = 0, updated; i < ANSWERS_PR_GROUP; i++){
            // wait for access to shared variable, mystery
            sem_wait(&mysteryKey);
            updated = mystery - 1;
            printf(ANSWER_MSG, group->name, mystery, updated);
            mystery = updated;
            sem_post(&mysteryKey);
            sleep(2);
        }

        // submit solution to instructor and leave
        sem_post(&submission);
        pthread_exit(&group->id);
    }
    else {
        sem_post(&chairsKey);
        printf(NO_FREE_CHAIR_MSG, group->name);
        pthread_exit(&group->id);
    }
}

/**
 * Instructor keeps checking until enough chairs have been taken.
 * Then the instructor invites n groups to escape room and releases mystery.
 * The instructor awaits the groups' submission, checks if the groups
 * solved the mystery, cleans up after them and does everything all over again.
 */
void *instructor_behaviour() {
    Instructor *instructor = getInstructor(pthread_self());

    while(1) {
        // wait for access to shared variable, chairs.taken
        sem_wait(&chairsKey);
        if (chairs.taken >= GROUPS_PR_INSTRUCTOR){
            Group inEscapeRoom[GROUPS_PR_INSTRUCTOR];
            sem_post(&chairsKey);

            // Invite n groups to escape room
            for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++){
                inEscapeRoom[i] = leave_chair(&chairs);
                printf(INSTRUCTOR_MSG, instructor->name, inEscapeRoom[i].name, chairs.taken, NUM_CHAIRS);
                sem_post(&availableInstructor);  // Become available to a group
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
            sem_post(&chairsKey);
        }
    }
}

/**
 * Prints goodbye message
 * @param status is winner or loser
 * @param group
 */
void goodbye(char *status, Group *group) {
    int groupIsWinner = strcmp(status, WINNER) == 0;

    if (groupIsWinner) printf("%s", CONGRATULATIONS_MSG);
    else printf("%s", UNLUCKY_MSG);

    for (int i = 0; i < GROUPS_PR_INSTRUCTOR; i++) printf(GROUP_NAME_MSG, group[i].name);

    if (groupIsWinner) printf("%s", MYSTERY_SOLVED_MSG);
    else printf("%s", MYSTERY_FAILED_MSG);
}

/**
 * Find instructor with pID
 * @param pID
 * @return group
 */
Instructor *getInstructor(pthread_t pID) {
    for (int i = 0; i < NUM_INSTRUCTORS; ++i)
        if (instructors[i].id == pID)
            return &instructors[i];
}

/**
 * Find group with pID
 * @param pID
 * @return group
 */
Group *getGroup(pthread_t pID) {
    for (int i = 0; i < NUM_GROUPS; ++i)
        if (groups[i].id == pID)
            return &groups[i];
}