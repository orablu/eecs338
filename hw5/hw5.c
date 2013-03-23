#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "common.h"
#include "hw5.h"
#include "oxygen.h"
#include "hydrogen.h"

// Macros
#define PCOUNT 15

// The semaphores.
enum SEMAPHORES {MUTEX = 0, O, H, B, SEMCOUNT};

// Keys for shared memory.
int shmid = -1;
int semkey = -1;

// Process ids.
pid_t pids[PCOUNT] = {0};

void main() {
    // Create shared memory.
    shmid = shmget(IPC_PRIVATE, sizeof(SHMMEM), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("Error getting shared memory");
        cleanup(EXIT_FAILURE);
    }

    // Create the semaphores.
    semkey = semget(IPC_PRIVATE, SEMCOUNT, IPC_CREAT | 0666);
    if (semkey < 0) {
        perror("Error getting semaphores");
        cleanup(EXIT_FAILURE);
    }
    initialize_semaphores(semkey);

    // Setup shared data.
    SHMDATA shmdata = {
        shmid, semkey,
        MUTEX, O, H, B
    };

    // Create processes.
    start_oxygen   (shmdata, 0);
    start_oxygen   (shmdata, 1);
    start_hydrogen (shmdata, 2);
    start_hydrogen (shmdata, 3);
    start_hydrogen (shmdata, 4);
    start_hydrogen (shmdata, 5);
    start_hydrogen (shmdata, 6);
    start_oxygen   (shmdata, 7);
    start_hydrogen (shmdata, 8);
    start_hydrogen (shmdata, 9);
    start_hydrogen (shmdata, 10);
    start_oxygen   (shmdata, 11);
    start_hydrogen (shmdata, 12);
    start_oxygen   (shmdata, 13);
    start_hydrogen (shmdata, 14);

    // Wait for children to complete.
    int i;
    int status = 0;
    int statusi;
    for (i = 0; i < PCOUNT; i++) {
        wait(&statusi);
        status = status || statusi;
    }

    // Wait for children.
    for (i = 0; i < PCOUNT; i++) {
        pids[i] = -1;
    }

    // All done!
    cleanup(status);

    // Unreachable, but the compiler wants it.
    return status;
}

void start_oxygen(SHMDATA shmdata, int ipid) {
    // Fork process.
    pid_t pid = tryfork();
    if (!pid) {
        return;
    }
    pids[ipid] = pid;

    oxygen(shmdata);
}

void start_hydrogen(SHMDATA shmdata, int ipid) {
    // Fork process.
    pid_t pid = tryfork();
    if (!pid) {
        return;
    }
    pids[ipid] = pid;

    hydrogen(shmdata);
}

pid_t tryfork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error forking");
        cleanup(EXIT_FAILURE);
    }
    return pid;
}

void initialize_semaphores(int semkey) {
    // Create union structure for counts
    SEMUN sem_union;
    unsigned short counters[4];
    counters[MUTEX] = 1;
    counters[O] = 0;
    counters[H] = 0;
    counters[B] = 1;
    sem_union.array = counters;

    // Call semctl to set all counts
    // (second argument is ignored)
    int semset = semctl(semkey, 0, SETALL, sem_union);
    if (semset < 0) {
        perror("Error setting semaphore counts");
        cleanup(EXIT_FAILURE);
    }
}

void cleanup(int status) {
    // Kill children if they're running
    int i;
    for (i = 0; i < PCOUNT; i++) {
        if (pids[i] > 0) {
            if (kill(pids[i], SIGKILL)) {
                perror("Error killing child process");
                status = EXIT_FAILURE;
            }
            wait(NULL);
        }
    }

    // For semaphore group with semkey
    // (Second argument ignored)
    // remove it (IPC_RMID)
    if (semkey >= 0) {
        if (semctl(semkey, 0, IPC_RMID) < 0) {
            perror("Error removing semaphores");
            status = EXIT_FAILURE;
        }
    }

    // For shared memory segment with id shmid
    // remove it (IPC_RMID)
    // (last argument ignored, use NULL pointer)
    if (shmid >= 0) {
        if (shmctl(shmid, IPC_RMID, (struct shmid_ds *) NULL)) {
            perror("Error removing shared memory");
            status = EXIT_FAILURE;
        }
    }

    exit(status);
}
