#include <time.h>

#include "smoker.h"

// Method primitives.
CLIENT * get_client(char * server_hostname);
void     destroy(CLIENT * client, int id);
void     mssleep(int ms);

int main(int argc, char**argv) {
	if(argc != 4) {
		printf("Usage: ./client HOSTNAME SMOKERID INCREMENT\n");
		return EXIT_FAILURE;
	}

	char *   server_hostname = argv[1];
    int      id              = atoi(argv[2]);
	int      increment       = atoi(argv[3]);
	CLIENT * client          = get_client(server_hostname);

    // Wait for other smokers to start.
    while (true) {
        struct smoker_id info = { id };
        if (smoker_start_1(&info, client)) {
            break;
        }
        mssleep(100);
    }

    // Begin smoking.
    int tobac = 0;
    int paper = 0;
    int match = 0;
    while (true) {
        // Check that we have enough tobacco.
        if (tobac <= 0) {
            struct smoker_info info = { TOBACCO, increment, id, 0 };
            int result = *smoker_proc_1(&info, client);
            if (result != ENOUGH) {
                printf("Not enough tobacco, and I will go kill myself!\n");
                destroy(client, id);
            }

            printf("Received %d tobacco!\n", increment);
        }

        // Check that we have enough paper.
        if (paper <= 0) {
            struct smoker_info info = { PAPER, increment, id, 0 };
            int result = *smoker_proc_1(&info, client);
            if (result != ENOUGH) {
                printf("Not enough paper, and I will go kill myself!\n");
                destroy(client, id);
            }

            printf("Received %d paper!\n", increment);
        }

        // Check that we have enough matches.
        if (match <= 0) {
            struct smoker_info info = { MATCHES, increment, id, 0 };
            int result = *smoker_proc_1(&info, client);
            if (result != ENOUGH) {
                printf("Not enough matches, and I will go kill myself!\n");
                destroy(client, id);
            }

            printf("Received %d matches!\n", increment);
        }

        //Smoke!
        printf("Smoking...\n");
        fflush(NULL);
        mssleep(50);
    }

    // Unreachable. Clean up just in case.
    destroy(client, id);
    return EXIT_FAILURE;
}

CLIENT * get_client(char * server_hostname) {
	CLIENT * client;
    client = clnt_create(server_hostname, SMOKER_PROG, SMOKER_VERS, "udp");
	if(client == NULL) {
		clnt_pcreateerror("Error creating client");
		exit(EXIT_FAILURE);
	}
    return client;
}

void destroy(CLIENT * client, int id) {
    struct smoker_id info = { id };
    smoker_exit_1(&info, client);
	clnt_destroy(client);
	return EXIT_SUCCESS;
}

void mssleep(int ms) {
    struct timespec time_ns;
    time_ns.tv_sec = 0;
    time_ns.tv_nsec = ms * 1000000;
    int result = nanosleep(&time_ns, NULL);
    if (result < 0) {
        perror("ERROR: Nanosleep");
        _exit(EXIT_FAILURE);
    }
}
