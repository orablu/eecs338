#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "common.h"
#include "oxygen.h"

#define SEMOP(x) try_semop(shared.semkey, &x)

void oxygen(SHMINFO shared) {
    // Define useful semaphore variables.
    SEMBUF   wait_mutex = {shared.mutex, WAIT,   0};
    SEMBUF signal_mutex = {shared.mutex, SIGNAL, 0};
    SEMBUF   wait_o = {shared.o, WAIT,   0};
    SEMBUF signal_o = {shared.o, SIGNAL, 0};
    SEMBUF   wait_h = {shared.h, WAIT,   0};
    SEMBUF signal_h = {shared.h, SIGNAL, 0};
    SEMBUF   wait_b = {shared.b, WAIT,   0};
    SEMBUF signal_b = {shared.b, SIGNAL, 0};

    // Attach to shared memory.
    SHMMEM * shmmem = shmat(shared.shmid, (void *)0, 0);

    log("BEGINNING OXYGEN PROCESS");

    SEMOP(wait_mutex);
    if (shmmem->HCount >= 2) {
        shmmem->BCount = 2;
        log("ENOUGH MOLECULES - SIGNALING AND EXITING");
        SEMOP(signal_h);
        SEMOP(signal_h);
        _exit(EXIT_SUCCESS);
    }
    else {
        shmmem->OCount++;
        log("LACKING MOLECULES - WAITING");
        SEMOP(signal_mutex);
        SEMOP(wait_o);
        SEMOP(wait_b);
        shmmem->OCount--;
        shmmem->BCount--;
        if (shmmem->BCount > 0) {
            log("AWAKENED - ENOUGH MOLECULES - SIGNALING AND EXITING");
            SEMOP(signal_b);
            _exit(EXIT_SUCCESS);
        }
        else {
            log("AWAKENED - ENOUGH MOLECULES - SIGNALING AND BONDING");
            SEMOP(signal_b);
            SEMOP(signal_mutex);
        }

        log("MADE IT ACROSS THE BARRIER AS H2O");
    }
}
