#include "smoker.h"

#define TOBACCO 0
#define PAPER   1
#define MATCHES 2

CLIENT * get_client(char * server_hostname);
void     destroy(CLIENT * client, int smoker_id);

int main(int argc, char**argv) {
	if(argc != 4) {
		printf("Usage: ./client HOSTNAME SMOKERID INCREMENT\n");
		return EXIT_FAILURE;
	}

	char *   server_hostname = argv[1];
    int      smoker_id       = atoi(argv[2]);
	int      increment       = atoi(argv[3]);
	CLIENT * client          = get_client(server_hostname);

    struct smoker_info info = { TOBACCO, increment, smoker_id, 0 };
	printf("Received. Done is %d\n", info.done);

    destroy(client, smoker_id);

    // Unreachable.
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

void destroy(CLIENT * client, int smoker_id) {
    struct exit_info info = { smoker_id };
    smoker_exit_1(&info, client);
	clnt_destroy(client);
	return EXIT_SUCCESS;
}
