#ifndef HW5_COMMON
#define HW5_COMMON

// Wait and signal operations, given to semop.
#define WAIT  -1
#define SIGNAL 1

// The shared memory information.
#define SHMINFO struct shminfo
SHMINFO {
    int shmid;
    int semkey;
    int mutex;
    int o;
    int h;
    int b;
}

// The shared memory buffer type.
#define SHMMEM struct shmmem
SHMMEM {
    int OCount;
    int HCount;
    int BCount;
};

// The sembuf structure.
#define SEMBUF struct sembuf

// The semun structure (copied from semctl man page)
#define SEMUN union semun
SEMUN {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

void try_semop(int semkey, SEMBUF * action);
void log(char * message);

#endif
