#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "hw02.h"

// Userful constants for file definitions
#define STDIN_FD  0
#define STDOUT_FD 1
#define REND 0
#define WEND 1

#define PIPE_ERROR "Error creating pipe %d"

#define PIPE(pipefd) do { if (pipe(pipefd)) { perror("Error creating pipe."); _exit(errno); } } while (0)
#define CLOSE(fd) do { int result = close(fd); if (result == -1) { perror("Error closing pipe."); _exit(errno); } } while (0)
#define DUP2(fd1, fd2) do { int result = dup2(fd1, fd2); if (result == -1) { perror("Error duplicating pipe."); _exit(errno); } } while (0)

// TODO: Close pipes as soon as done writing!

int main() {
    char str[BUFSIZ];
    ssize_t len = read(STDIN_FD, str, BUFSIZ);

    // Only reading one line, stop at the first newline.
    int i;
    for (i = 0; i < len; i++) {
        if (str[i] == '\n') {
            str[len - 1] = '\0';
            len = i;
            break;
        }
    }

    fprintf(stderr, "Entered main. String is %s, length is %d.\n", str, len);
    fprintf(stderr, "Strlen says %d.\n", strlen(str));
    fflush(stderr);

    if (len <= 2) {
        // Only one actual letter, print and be done.
        fprintf(stdout, "%s", str);
        fflush(stdout);
        return 0;
    }

    // Begin the recursion.
    mergeSort(str, strlen(str));

    // All done!
    return 0;
}

void mergeSort(char *str, int len) {
    // Print out the string we have.
    fprintf(stderr, "Process entered mergeSort. String is \"%s\".\n", str);
    fflush(stderr);

    // Create pipes.
    int pipefd0[2]; int pipefd1[2]; int pipefd2[2]; int pipefd3[2];
    PIPE(pipefd0);  PIPE(pipefd1);  PIPE(pipefd2);  PIPE(pipefd3);

    /// LEFT SUBSTRING

    pid_t l_pid = fork();
    if (l_pid < 0) {
        perror("Error forking left child.");
        _exit(EXIT_FAILURE);
    }
    else if (l_pid == 0) {
        // Close pipes R123, W023
        CLOSE(pipefd1[REND]); CLOSE(pipefd2[REND]); CLOSE(pipefd3[REND]);
        CLOSE(pipefd0[WEND]); CLOSE(pipefd2[WEND]); CLOSE(pipefd3[WEND]);

        // Pipes used are 0 -> stdin, 1 -> stdout
        readChild(pipefd0, pipefd1, "Left");

        // All done for this thread!
        return;
    }

    fprintf(stderr, "Process created left child.\n");
    fflush(stderr);

    /// RIGHT SUBSTRING

    pid_t r_pid = fork();
    if (r_pid < 0) {
        perror("Error forking right child.");
        _exit(EXIT_FAILURE);
    }
    else if (r_pid == 0) {
        // Close pipes R013, W012
        CLOSE(pipefd0[REND]); CLOSE(pipefd1[REND]); CLOSE(pipefd3[REND]);
        CLOSE(pipefd0[WEND]); CLOSE(pipefd1[WEND]); CLOSE(pipefd2[WEND]);

        // Pipes used are 2 -> stdin, 3 -> stdout
        readChild(pipefd2, pipefd3, "Right");

        // All done for this thread!
        return;
    }

    fprintf(stderr, "Process created right child.\n");
    fflush(stderr);

    /// PARENT STRING

    // Close pipes R13, W24
    CLOSE(pipefd0[REND]); CLOSE(pipefd2[REND]);
    CLOSE(pipefd1[WEND]); CLOSE(pipefd3[WEND]);

    // Pipes used for left are 1 -> in, 0 -> out. Pipes used for right are 3 -> in, 2 -> out.

    // Divide the string into half, one for each side.
    int splitllen = len / 2; // One less because of the null character.
    int splitrlen = len - (splitllen); // The rest of the string, less the null character.
    char * strleft = malloc(splitllen + 1); strleft[splitllen] = '\0';
    char * strright = malloc(splitrlen + 1); strright[splitrlen] = '\0';
    strncpy(strleft, str, splitllen);
    strncpy(strright, str + splitllen, splitrlen);

    fprintf(stderr, "Process splitting string.\n\tLeft is \"%s\", right is \"%s\".\n", strleft, strright);
    fflush(stderr);

    // Write out the left string.
    write(pipefd0[WEND], strleft, strlen(strleft));
    close(pipefd0[WEND]);
    free(strleft);

    // Write out the right string.
    write(pipefd2[WEND], strright, strlen(strright));
    close(pipefd2[WEND]);
    free(strright);

    fprintf(stderr, "Process waiting for children. String is \"%s\".\n", str);
    fflush(stderr);

    // Wait for response.
    int statusl; int statusr;
    pid_t resultl = wait(&statusl);
    pid_t resultr = wait(&statusr);
    if (resultl < 0 || resultr < 0) {
        perror("A child terminated unsucessfully.");
        _exit(EXIT_FAILURE);
    }

    // Read the response.
    char lres[BUFSIZ];
    ssize_t llen = read(pipefd1[REND], lres, strlen(lres));
    CLOSE(pipefd1[REND]);
    char rres[BUFSIZ];
    ssize_t rlen = read(pipefd3[REND], rres, strlen(rres));
    CLOSE(pipefd3[REND]);

    fprintf(stderr, "Process read results. Left result is \"%s\", right result is \"%s\".\n", lres, rres);
    fflush(stderr);

    // Merge the strings back together.
    char * mergedstring;
    int mergedlen = sort(mergedstring, lres, rres, llen, rlen);

    // Return the result to the parent.
    fprintf(stderr, "Process merged string. String is \"%s\".\n", mergedstring);
    fflush(stderr);

    fprintf(stdout, "%s", mergedstring);
    fflush(stdout);
    free(mergedstring);

    // All done for this thread!
    _exit(EXIT_SUCCESS);
}

void readChild(int * pipefdin, int * pipefdout, char * name) {
    // Print out that we entered.
    fprintf(stderr, "Process entered %s child. Waiting for string.\n", name);
    fflush(stderr);

    // Change pipes to stedin and stdout.
    CLOSE(STDIN_FD);
    DUP2(pipefdin[REND], STDIN_FD);

    CLOSE(STDOUT_FD);
    DUP2(pipefdout[WEND], STDOUT_FD);

    // We're not done, continue
    execlp("./main", "./main", NULL);
}

// TODO: Remember to free the result when done!
int sort(char * mergedstring, char * left, char * right, int llen, int rlen) {
    // We need one less than the sum of the sizes (only need one null terminator).
    if (llen + rlen < 1) {
        mergedstring = malloc(1);
        mergedstring[0] = '\0';
        return 1;
    }

    mergedstring = malloc(llen + rlen - 1);
    mergedstring[llen + rlen - 2] = '\0';

    int i = 0;
    int l = 0; int lmax = llen - 1;
    int r = 0; int rmax = rlen - 1;
    while (l < lmax || r < rmax) {
        if (l < lmax && r < rmax) {
            if (left[l] <= right[r]) {
                mergedstring[i++] = left[l++];
            }
            else {
                mergedstring[i++] = right[r++];
            }
        }
        else if (l < lmax) {
            mergedstring[i++] = left[l++];
        }
        else if (r < rmax) {
            mergedstring[i++] = right[r++];
        }
    }

    return strlen(mergedstring);
}
