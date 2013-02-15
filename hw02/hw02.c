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

// TODO: Close pipes as soon as done writing!

int main() {
    char str[BUFSIZ];
    ssize_t len = read(STDIN_FD, str, BUFSIZ);

    // Only reading one line, stop at the first newline.
    int i;
    for (i = 0; i < len; i++) {
        if (str[i] == '\n') {
            len = i + 1;
            break;
        }
    }

    // Make sure the string terminates!
    str[len - 1] = '\0';

    // Begin the recursion.
    mergeSort(str, len + 1);

    // All done!
    printf("\n");
    return 0;
}

void mergeSort(char *str, int len) {
    // Print out the string we have.
    fprintf(stderr, "Process entered mergeSort. String is \"%s\".\n", str);

    // Create pipes.
    int pipefd0[2], pipefd1[2], pipefd2[2], pipefd3[2];
    PIPE(pipefd0); PIPE(pipefd1); PIPE(pipefd2); PIPE(pipefd3);

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

    /// RIGHT SUBSTRING

    pid_t r_pid = fork();
    if (r_pid < 0) {
        perror("Error forking right child.");
        _exit(EXIT_FAILURE);
    }
    else if (l_pid == 0) {
        // Close pipes R013, W012
        CLOSE(pipefd0[REND]); CLOSE(pipefd1[REND]); CLOSE(pipefd3[REND]);
        CLOSE(pipefd0[WEND]); CLOSE(pipefd1[WEND]); CLOSE(pipefd2[WEND]);

        // Pipes used are 2 -> stdin, 3 -> stdout
        readChild(pipefd2, pipefd3, "Right");

        // All done for this thread!
        return;
    }

    fprintf(stderr, "Process created right child.\n");

    /// PARENT STRING

    // Close pipes R13, W24
    close(pipefd0[REND]); close(pipefd2[REND]);
    close(pipefd1[WEND]); close(pipefd3[WEND]);

    // Pipes used for left are 1 -> in, 0 -> out. Pipes used for right are 3 -> in, 2 -> out.

    // Divide the string into half, one for each side.
    char * strleft = malloc((len / 2) + 1); // Add one for the new null character.
    char * strright = malloc(len - (len / 2)); // Round up for the rest of the string.
    strncpy(strleft, str, len / 2); strleft[len / 2] = '\0'; // Copy the first half, and add ther null terminator.
    strncpy(strright, str + (len / 2), len - (len / 2)); // Copy the second half.

    fprintf(stderr, "Process splitting string.\n\tLeft is \"%s\", right is \"%s\".\n", strleft, strright);

    // Write out the left string.
    write(pipefd0[WEND], strleft, (len / 2) + 1);
    close(pipefd0[WEND]);
    free(strleft);

    // Write out the right string.
    write(pipefd2[WEND], strright, len - (len / 2));
    close(pipefd2[WEND]);
    free(strright);

    fprintf(stderr, "Process waiting for children. String is \"%s\".\n", str);

    // Wait for response.
    int statusl, statusr;
    pid_t resultl = wait(&statusl);
    pid_t resultr = wait(&statusr);
    if (resultl < 0 || resultr < 0) {
        perror("A child terminated unsucessfully.");
        _exit(EXIT_FAILURE);
    }

    // Read the response.
    char * lres = malloc((len / 2) + 1);
    ssize_t llen = read(pipefd1[REND], lres, (len / 2) + 1);
    char * rres = malloc(len - (len / 2));
    ssize_t rlen = read(pipefd3[REND], rres, len - (len / 2));

    fprintf(stderr, "Process read results. Left result is \"%s\", right result is \"%s\".\n", lres, rres);

    // Merge the strings back together.
    char * mergedstring = sort(lres, rres, (len / 2) + 1, len - (len / 2));
    free(lres); free(rres); // Free unneeded substrings.

    // Return the result to the parent.
    fprintf(stderr, "Process merged string. String is \"%s\".\n", mergedstring);
    printf(mergedstring);
    free(mergedstring);

    // All done for this thread!
    _exit(EXIT_SUCCESS);
}

void readChild(int * pipefdin, int * pipefdout, char * name) {
    // Print out that we entered.
    fprintf(stderr, "Process entered %s child. Waiting for string.\n", name);

    // Change pipes to stedin and stdout.
    CLOSE(STDIN_FD);
    dup2(pipefdin[REND], STDIN_FD);

    CLOSE(STDOUT_FD);
    dup2(pipefdout[WEND], STDOUT_FD);

    // Get the present substring.
    char buf[BUFSIZ];
    ssize_t strlen = read(STDIN,_FD buf, BUFSIZ);

    fprintf(stderr, "%s child read string. String is \"%s\", length is %d.\n", name, buf, strlen);

    // Are we done? (Don't forget one of the characters is the null terminator!)
    if (strlen <= 2) {
        // add a null character to make it a string.
        buf[1] = '\0';

        // Return the value to the parent.
        printf("%s", buf);
    }
    else {
        // We're not done, continue
        execlp("./main", "./main", NULL);
    }
}

// TODO: Remember to free the result when done!
char * sort(char * left, char * right, int llen, int rlen) {
    // We need one less than the sum of the sizes (only need one null terminator).
    char * merge = malloc(llen + rlen - 1);
    merge[llen + rlen - 2] = '\0';

    int i = 0;
    int l = 0; int lmax = llen - 1;
    int r = 0; int rmax = rlen - 1;
    while (l < lmax || r < rmax) {
        if (l < lmax && r < rmax) {
            if (left[l] <= right[r]) {
                merge[i++] = left[l++];
            }
            else {
                merge[i++] = right[r++];
            }
        }
        else if (l < lmax) {
            merge[i++] = left[l++];
        }
        else if (r < rmax) {
            merge[i++] = right[r++];
        }
    }

    return merge;
}
