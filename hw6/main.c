/* Wes Rupert - wkr3 *
 * EECS 338   - hw6  */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#include "main.h"

#define ATOBCOUNT  25
#define BTOACOUNT  25
#define MAXSLEEP   1000
#define LOG(message) printf("THREAD %d:\t Xing=%d, Xed=%d, ToBW=%d, ToAW=%d, Dir=%d,\t%s\n", id, XingCount, XedCount, ToBWaitCount, ToAWaitCount, (int)XingDirection, message)

// Semaphores.
sem_t mutex;
sem_t ToB;
sem_t ToA;

// Shared memory.
int XingCount = 0;
int XedCount = 0;
int ToBWaitCount = 0;
int ToAWaitCount = 0;
enum { NONE = 0, ATOB, BTOA } XingDirection = NONE;

// Thread Ids for the baboons.
#define THREADINFO struct threadinfo
THREADINFO { int threadId; };
THREADINFO atobIds[ATOBCOUNT];
THREADINFO btoaIds[BTOACOUNT];

int main(void) {
    // Thread information.
    pthread_t atobBaboons[ATOBCOUNT];
    pthread_t btoaBaboons[BTOACOUNT];
    pthread_attr_t attr;
    void * status;
    int i;

    // Seed the random number generator.
    srand(time(NULL));

    // Create a set of attributes for the threads.
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Initialize the semaphores.
    sem_init(&mutex, 0, 1);
    sem_init(&ToB, 0, 0);
    sem_init(&ToA, 0, 0);

    /// Spawn baboons.

    // A to B baboons.
    for (i = 0; i < ATOBCOUNT; i++) {
        atobIds[i].threadId = i;
        int result = pthread_create(&atobBaboons[i], &attr, atobCross, (void *)&atobIds[i]);
        if (result < 0) {
            perror("ERROR: Thread creation A to B");
            _exit(EXIT_FAILURE);
        }
    }

    // B to A baboons.
    for (i = 0; i < ATOBCOUNT; i++) {
        btoaIds[i].threadId = i;
        int result = pthread_create(&btoaBaboons[i], &attr, btoaCross, (void *)&btoaIds[i]);
        if (result < 0) {
            perror("ERROR: Thread creation B to A");
            _exit(EXIT_FAILURE);
        }
    }

    /// Wait for threads to finish.

    // A to B baboons.
    for (i = 0; i < ATOBCOUNT; i++) {
        int result = pthread_join(atobBaboons[i], &status);
        if (result < 0) {
            perror("ERROR: Thread join A to B");
            _exit(EXIT_FAILURE);
        }
    }

    // B to A baboons.
    for (i = 0; i < BTOACOUNT; i++) {
        int result = pthread_join(btoaBaboons[i], &status);
        if (result < 0) {
            perror("ERROR: Thread join B to A");
            _exit(EXIT_FAILURE);
        }
    }

    // All done!
    return EXIT_SUCCESS;
}

void * atobCross(void * threadId) {
    THREADINFO * info;
    info = (THREADINFO *)threadId;
    int id = info->threadId;

    // Simulate being idle for a while.
    randSleep(MAXSLEEP);

    LOG("Approaching crossing");

    sem_wait(&mutex);

    if ((XingDirection == ATOB || XingDirection == NONE) && XingCount < 5 && (XedCount + XingCount) < 10) {
        LOG("Can cross");
        XingDirection = ATOB;
        XingCount++;
        LOG("LEAVING CS");
        sem_post(&mutex);
    }
    else {
        LOG("Unable to cross");
        ToBWaitCount++;

        LOG("LEAVING CS");
        sem_post(&mutex);

        LOG("Waiting for ToB");
        sem_wait(&ToB);
        LOG("Got ToB");

        ToBWaitCount--;
        XingCount++;
        XingDirection = ATOB;

        LOG("LEAVING CS");
        sem_post(&mutex);
    }

    // Simulate crossing.
    LOG("Crossing");
    randSleep(MAXSLEEP);

    // Done crossing.
    LOG("Waiting for CS");
    sem_wait(&mutex);
    LOG("ENTERED CS");

    LOG("Finished crossing");
    XedCount++;
    XingCount--;

    if (ToBWaitCount != 0 && (((XedCount + XingCount) < 10) || ((XedCount + XingCount) >= 10 && ToAWaitCount == 0))) {
        LOG("Signalling more A to B");
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount != 0 && (ToBWaitCount == 0 || (XedCount + XingCount) >= 10)) {
        LOG("Switching crossing control to B to A");
        XingDirection = BTOA;
        XedCount = 0;
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount == 0 && ToAWaitCount == 0) {
        LOG("Switching control to NONE");
        XingDirection = NONE;
        XedCount = 0;
        LOG("LEAVING CS");
        sem_post(&mutex);
    }
    else {
        LOG("LEAVING CS");
        sem_post(&mutex);
    }

    LOG("CROSSING COMPLETE - EXITING");

    return EXIT_SUCCESS;
}

void * btoaCross(void * threadId) {
    THREADINFO * info;
    info = (THREADINFO *)threadId;
    int id = info->threadId;

    // Simulate being idle for a while.
    randSleep(MAXSLEEP);

    LOG("Approaching crossing");

    sem_wait(&mutex);

    if ((XingDirection == BTOA || XingDirection == NONE) && XingCount < 5 && (XedCount + XingCount) < 10) {
        LOG("Can cross");
        XingDirection = BTOA;
        XingCount++;
        LOG("LEAVING CS");
        sem_post(&mutex);
    }
    else {
        LOG("Unable to cross");
        ToAWaitCount++;

        LOG("LEAVING CS");
        sem_post(&mutex);

        LOG("Waiting for ToA");
        sem_wait(&ToA);
        LOG("Got ToA");

        ToAWaitCount--;
        XingCount++;
        XingDirection = BTOA;

        LOG("LEAVING CS");
        sem_post(&mutex);
    }

    // Simulate crossing.
    LOG("Crossing");
    randSleep(MAXSLEEP);

    // Done crossing.
    LOG("Waiting for CS");
    sem_wait(&mutex);
    LOG("ENTERED CS");

    LOG("Finished crossing");
    XedCount++;
    XingCount--;

    if (ToAWaitCount != 0 && (((XedCount + XingCount) < 10) || ((XedCount + XingCount) >= 10 && ToBWaitCount == 0))) {
        LOG("Signalling more A to B");
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount != 0 && (ToAWaitCount == 0 || (XedCount + XingCount) >= 10)) {
        LOG("Switching crossing control to B to A");
        XingDirection = ATOB;
        XedCount = 0;
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount == 0 && ToBWaitCount == 0) {
        LOG("Switching control to NONE");
        XingDirection = NONE;
        XedCount = 0;
        LOG("LEAVING CS");
        sem_post(&mutex);
    }
    else {
        LOG("LEAVING CS");
        sem_post(&mutex);
    }

    LOG("CROSSING COMPLETE - EXITING");

    return EXIT_SUCCESS;
}

void randSleep(int limit) {
    int duration = rand() % limit + 1;

    struct timespec time_ns;
    time_ns.tv_sec = 0;
    time_ns.tv_nsec = duration * 1000000;

    int result = nanosleep(&time_ns, NULL);
    if (result < 0) {
        perror("ERROR: Nanosleep");
        _exit(EXIT_FAILURE);
    }
}

