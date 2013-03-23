#ifndef HW5_MAIN
#define HW5_MAIN

void main();
void start_oxygen(SHMDATA shmdata, int ipid);
void start_hydrogen(SHMDATA shmdata, int ipid);
pid_t tryfork();
void initialize_semaphores(int semkey);
void cleanup(int status);

#endif
