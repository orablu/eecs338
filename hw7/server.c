#include "smoker.h"

#define TOBACCO 0
#define PAPER   1
#define MATCHES 2

#define STR_TOBAC "tobacco"
#define STR_PAPER "paper."
#define STR_MATCH "matches."
#define STR_UNKNO "of an unknown substance."

char client_active[3] = { 0, 0, 0 };
char active = 0;

void _log(smoker_info * in);

void * smoker_proc_1_svc(struct smoker_info * in, struct svc_req * rqstp)
{
    if (!client_active[in->id]) {
        client_active[in->id] = 1;
        ++active;
    }

    _log(in);

    in->done = 7;

	fflush(NULL);
}

void * smoker_exit_1_svc(struct exit_info * in, struct svc_req * rqstp)
{
	printf("Request for Termination Received\n");
	fflush(NULL);

    if (client_active[in->id]) {
        client_active[in->id] = 0;
        --active;
    }

    if (!active) {
        exit(EXIT_SUCCESS);
    }
}

void _log(smoker_info * in) {
    printf("Server Receiving request from smoker %d for %d ",
            in->id,
            in->amount);
    switch (in->material) {
        case TOBACCO :
            printf(STR_TOBAC);
            break;
        case PAPER :
            printf(STR_PAPER);
            break;
        case MATCHES :
            printf(STR_MATCH);
            break;
        default :
            printf(STR_UNKNO);
            break;
    }
}

