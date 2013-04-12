#include "smoker.h"

int main(int argc, char**argv)
{
	CLIENT *cl;

	if(argc != 3)
	{
		printf("Usage: ./client HOSTNAME i\n");
		return 1;
	}

	char *server_hostname = argv[1];
	cl = clnt_create(server_hostname, SMOKER_PROG, SMOKER_VERS, "udp");
	if(cl == NULL) {
		clnt_pcreateerror("Error creating client");
		exit(EXIT_FAILURE);
	}

	struct smoker_info t = {atoi(argv[2]), 9999};

	if(t.one == -1)
	{
		smoker_exit_1(&t, cl);
		printf("Terminated Server\n");
		return 0;
	}
	int* result = smoker_proc_1(&t, cl);
	printf("Received result %d\n", *result);
	clnt_destroy(cl);
	return 0;
}
