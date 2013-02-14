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
#define BUFSIZE 1024

// TODO: Close pipes as soon as done writing!

int main() {

    char str[BUFSIZE];
    ssize_t len = read(STDIN_FD, str, BUFSIZE);
    str[len] = '\0';

    mergeSort(str, len + 1);

    return EXIT_SUCCESS;
}

void mergeSort(char *str, int len) {
    // Chop our string to the given length. TODO: Stop doing this.
    str[len - 1] = '\0';
    
    // Print out the string we have.
    fprintf(stderr, "Process entered mergeSort. String is \"%s\".", str);

    // Create pipes.
    int pipefd[4][2];
    int i;
    for (i = 0; i < 4; ++i) {
        // Open the pipe.
        if (pipe(pipefd[i])) {
            char buf[32];
            sprintf(buf, PIPE_ERROR, (i + 1));
            perror(buf);
            _exit(EXIT_FAILURE);
        }
    }

    /// LEFT SUBSTRING

    pid_t l_pid = fork();
    if (l_pid < 0) {
        perror("Error forking left child");
        _exit(EXIT_FAILURE);
    }
    else if (l_pid == 0) {
        // Left child process. Change pipe information, then repeat function.

        // Close pipes R123, W023
        close(pipefd[1][REND]); close(pipefd[2][REND]); close(pipefd[3][REND]);
        close(pipefd[0][WEND]); close(pipefd[2][WEND]); close(pipefd[3][WEND]);

        // Pipes used are 0 -> stdin, 1 -> sdtout
        close(STDIN_FD);
        dup2(pipefd[0][REND], STDIN_FD);

        close(STDOUT_FD);
        dup2(pipefd[1][WEND], STDOUT_FD);


        // Get the present substring.
        char buf[BUFSIZE];
        ssize_t strlen = read(stdin, buf, BUFSIZE);

        // Are we done? (Don't forget one of the characters is the null terminator!)
        if (strlen <= 2) {
            // add a null character to make it a string.
            buf[1] = '\0';

            // Return the value to the parent.
            printf("%s", buf);
        }
        else {
            // We're not done, continue
            mergeSort(buf, strlen);
        }

        // All done for this thread!
        _exit(EXIT_SUCCESS);
    }

    /// RIGHT SUBSTRING

    pid_t r_pid = fork();
    if (r_pid < 0) {
        perror("Error forking right child");
        _exit(EXIT_FAILURE);
    }
    else if (l_pid == 0) {
        // Right child process. Change pipe information, then repeat function.

        // Close pipes R013, W012
        close(pipefd[0][REND]); close(pipefd[1][REND]); close(pipefd[3][REND]);
        close(pipefd[0][WEND]); close(pipefd[1][WEND]); close(pipefd[2][WEND]);

        // Pipes used are 2 -> stdin, 3 -> sdtout
        close(STDIN_FD);
        dup2(pipefd[2][REND], STDIN_FD);

        close(STDOUT_FD);
        dup2(pipefd[3][WEND], STDOUT_FD);

        // Get the present substring.
        char buf[BUFSIZE];
        ssize_t strlen = read(stdin, buf, BUFSIZE);

        // Are we done? (Don't forget one of the characters is the null terminator!)
        if (strlen <= 2) {
            // Return the value to the parent.
            printf("%s", buf);
        }
        else {
            // We're not done, continue
            mergeSort(buf, strlen);
        }
        
        // All done for this thread!
        _exit(EXIT_SUCCESS);
    }

    // Close pipes R13, W24
    close(pipefd[0][REND]); close(pipefd[2][REND]);
    close(pipefd[1][WEND]); close(pipefd[3][WEND]);

    // Pipes used for left are 1 -> in, 0 -> out. Pipes used for right are 3 -> in, 2 -> out.

    // Divide the string into half, one for each side.
    char * strleft = malloc((len / 2) + 1); // Add one for the new null character.
    char * strright = malloc(len - (len / 2)); // Round up for the rest of the string.
    strncpy(strleft, str, len / 2); strleft[len / 2] = '\0'; // Copy the first half, and add ther null terminator.
    strncpy(strright, str + (len / 2) + 1, len - (len / 2)); // Copy the second half.

    // Write out the left string.
    write(pipefd[0][WEND], strleft, (len / 2) + 1);
    close(pipefd[0][WEND]);
    free(strleft);

    // Write out the right string.
    write(pipefd[2][WEND], strright, len - (len / 2));
    close(pipefd[2][WEND]);
    free(strright);

    // Wait for response.
    int statusl, statusr;
    pid_t resultl = wait(&statusl);
    pid_t resultr = wait(&statusr);
    if (resultl < 0 || resultr < 0) {
        perror("A child terminated unsucessfully");
        _exit(EXIT_FAILURE);
    }

    // Read the response.
    char * lres = malloc((len / 2) + 1); // TODO: FREE
    ssize_t llen = read(pipefd[1][REND], lres, (len / 2) + 1);
    char * rres = malloc(len - (len / 2)); // TODO: FREE
    ssize_t rlen = read(pipefd[3][REND], rres, len - (len / 2));

    // Merge the strings back together.
    char * mergedstring = sort(lres, rres, (len / 2) + 1, len - (len / 2));
    free(lres); free(rres); // Free unneeded substrings.

    // Return the result to the parent.
    printf(mergedstring);
    free(mergedstring);

    // All done for this thread!
    _exit(EXIT_SUCCESS);
}

// Remember to free the result when done!
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
