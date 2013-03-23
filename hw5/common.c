#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include "common.h"
#include "hw5.h"
#include "oxygen.h"
#include "hydrogen.h"

void try_semop(int semkey, SEMBUF * action) {
    if (semop(semkey, action, 1) < 0) {
        perror("Semop error");
        _exit(EXIT_FAILURE);
    }
}

void log(char * message) {
    printf("PROCESS %d: %s\n",
           0/* TODO: pid */,
           message);
}
