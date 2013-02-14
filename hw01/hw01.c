#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "hw01.h"

char * ePAPER = "PAPER=%d";
char * eTOBAC = "TOBACCO=%d";
char * eMATCH = "MATCHES=%d";
char * sPAPER = "PAPER";
char * sTOBAC = "TOBACCO";
char * sMATCH = "MATCHES";

int main() {

    int p[] = { 1, 2, 3 };
    int t[] = { 3, 3, 2 };
    int m[] = { 4, 3, 3 };

    int result = -1;

    int i;
    for (i = 0; i < 3; ++i) {
        printf("STARTING AGENT\n\n");
        result = agent(p[i], t[i], m[i]);
        printf("\nAGENT FINISHED. RESULT: %d\n", result);

        if (result != 0) {
            exit(result);
        }
    }

    exit(result);
}

int agent(int p, int t, int m) {
    char name[256];
    if (gethostname(&name, sizeof(name))) {
        printf("Gethostname failed!");
        exit(1);
    }

    time_t thetime = time(NULL);

    // Print out agent information
    printf("AGENT INFO:");
    printf("\n HOSTNAME: %s", name);
    printf("\n PID:      %d", getpid());
    printf("\n UID:      %d", getuid());
    printf("\n TIME:     %s", ctime(&thetime));
    printf("\n");

    // Set the environment variables
    char paper[16];
    sprintf(paper, ePAPER, p);
    char tobac[16];
    sprintf(tobac, eTOBAC, t);
    char match[16];
    sprintf(match, eMATCH, m);
    putenv(paper);
    putenv(tobac);
    putenv(match);

    // Fork time!
    printf("FORKING THREE SMOKERS!\n");
    int result = forkSmokers();

    return result;
}

int forkSmokers() {
    pid_t process_id1 = fork();

    if (process_id1 < 0) {
        // Process creation failed
        return -1;
    }
    else if (process_id1 > 0) {
        // This is the parent process
        printf("SMOKER S1\n");

        pid_t process_id2 = fork();

        if (process_id2 < 0) {
            // Process creation failed
            return -1;
        }
        else if (process_id2 > 0) {
            // This is the parent process
            printf("SMOKER S2\n");

            pid_t process_id3 = fork();

            if (process_id3 < 0) {
                // Process creation failed
                return -1;
            }
            else if (process_id3 > 0) {
                // This is the parent process
                printf("SMOKER S3\n");

                int status1, status2, status3;
                //waitpid(process_id1, &status1, 0 ^ WIFEXITED);
                wait(&status1);
                wait(&status2);
                wait(&status3);

                printf ("AGENT IS DONE!\n");
                return 0;
            }
            else {
                // process_id == 0, this is the child process
                // Smoker S3
                sleep(2);

                // Decrement for the other smokers.
                decrementEnvs(2);

                while (1) {
                    smoke();
                }
            }
        }
        else {
            // process_id == 0, this is the child process
            // Smoker S2
            sleep(1);

            // Decrement for the other smokers.
            decrementEnvs(1);

            while (1) {
                smoke();
            }
        }
    }
    else {
        // process_id == 0, this is the child process
        // Smoker S1
        while (1) {
            smoke();
        }
    }

    return 0;
}

void smoke() {
    printf("SMOKER: %d\n", getpid());
    /// PAPER

    // Get the paper.
    int P = atoi(getenv(sPAPER));
    if (P <= 0) {
        printf("NO PAPER!\n");
        _exit(getpid());
    }

    // Decrement the paper.
    char paper[16];
    sprintf(paper, ePAPER, --P);
    putenv(paper);
    printf("PAPER AVAILABLE! P: %d\n", P);

    /// TOBACCO

    // Get the tobacco.
    int T = atoi(getenv(sTOBAC));
    if (T <= 0) {
        printf("NO TOBACCO!\n");
        _exit(getpid());
    }

    // Decrement the tobacco.
    char tobac[16];
    sprintf(tobac, eTOBAC, --T);
    putenv(tobac);
    printf("TOBACCO AVAILABLE! T: %d\n", T);

    /// MATCHES

    // Get the matches.
    int M = atoi(getenv(sMATCH));
    if (M <= 0) {
        printf("NO MATCHES!\n");
        _exit(getpid());
    }

    // Decrement the matches.
    char match[16];
    sprintf(match, eMATCH, --M);
    putenv(match);
    printf("MATCHES AVAILABLE! M: %d\n", M);

    fflush(stdout);

    // Sleep a bit.
    sleep(3);

    // Decrement for the other smokers.
    decrementEnvs(2);
}

void decrementEnvs(int value) {
    //return; // FRIG IT FAILS!
    char temp1[16];
    sprintf(temp1, ePAPER, atoi(getenv(sPAPER)) - value);
    putenv(temp1);
    char temp2[16];
    sprintf(temp2, eTOBAC, atoi(getenv(sTOBAC)) - value);
    putenv(temp2);
    char temp3[16];
    sprintf(temp3, eMATCH, atoi(getenv(sMATCH)) - value);
    putenv(temp3);
}
