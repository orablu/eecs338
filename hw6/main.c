#include <stdlib.h>
#include <stio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#include "main.h"

#define ATOBCOUNT  25
#define BTOACOUNT  25
#define MAXSLEEP   1000
#define LOG(message) printf("THREAD %d: %s\n", threadId, message)

// Semaphores.
sem_t mutex;
sem_t ToB;
sem_t ToA;

// Shared memory.
int XingCount = 0;
int XedCount = 0;
int ToBWaitCount = 0;
int ToAWaitCount = 0;
enum DIRECTIONS { NONE = 0, ATOB, BTOA } XingDirection = NONE;

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
    srandom(time(NULL));

    // Create a set of attributes for the threads.
    pthread_attr_init(&attr);
    pthread_attr_settdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Initialize the semaphores.
    sem_init(mutex, 0, 1);
    sem_init(ToB, 0, 0);
    sem_init(ToA, 0, 0);

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
        LOG("Can cross")
        XingDirection = ATOB;
        XingCount++;
        sem_post(&mutex);
    }
    else {
        LOG("Unable to cross - waiting");
        ToBWaitCount++;
        sem_post(&mutex);
        sem_wait(&ToB);
    }

    // Simulate crossing.
    LOG("Crossing");
    randSleep(MAXSLEEP);

    sem_wait(&mutex);

    // Done crossing.
    LOG("Finished crossing");
    XedCount++;
    XingCount--;

    if (ToBWaitCount != 0 && (((XedCount + XingCount) < 10) || ToAWaitCount == 0)) {
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount != 0 && (ToBWaitCount == 0 || (XedCount + XingCount) >= 10)) {
        XingDirection = BTOA;
        XedCount = 0;
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount == 0 && ToAWaitCount == 0) {
        XingDirection = NONE;
        XedCount = 0;
        sem_post(&mutex);
    }
    else {
        sem_post(&mutex);
    }
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
        LOG("Can cross")
        XingDirection = BTOA;
        XingCount++;
        sem_post(&mutex);
    }
    else {
        LOG("Unable to cross - waiting");
        ToAWaitCount++;
        sem_post(&mutex);
        sem_wait(&ToB);
    }

    // Simulate crossing.
    LOG("Crossing");
    randSleep(MAXSLEEP);

    sem_wait(&mutex);

    // Done crossing.
    LOG("Finished crossing");
    XedCount++;
    XingCount--;

    if (ToAWaitCount != 0 && (((XedCount + XingCount) < 10) || ToBWaitCount == 0)) {
        sem_post(&ToA);
    }
    else if (XingCount == 0 && ToBWaitCount != 0 && (ToAWaitCount == 0 || (XedCount + XingCount) >= 10)) {
        XingDirection = ATOB;
        XedCount = 0;
        sem_post(&ToB);
    }
    else if (XingCount == 0 && ToAWaitCount == 0 && ToBWaitCount == 0) {
        XingDirection = NONE;
        XedCount = 0;
        sem_post(&mutex);
    }
    else {
        sem_post(&mutex);
    }
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
