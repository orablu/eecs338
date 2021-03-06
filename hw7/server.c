#include "common.h"
#include "smoker.h"

char materials[3] = {
    25, // TOBACCO
    21, // PAPER
    20  // MATCHES
};

// Determines whether there are still active smokers or not.
char smoker_active[SMOKERCOUNT] = { 0, 0, 0 };
char active = 0;

// Method primitives.
void _log(smoker_info * in);

int * smoker_start_1_svc(struct smoker_id * in, struct svc_req * rqstp) {
    static int result;
    int newline = 0;

    if (in->logging)
        printf("Queried to start by smoker %d.", in->id);

    // Add the smoker to the active pool if necessary.
    if (!smoker_active[in->id]) {
        smoker_active[in->id] = 1;
        ++active;
        printf(" Activating smoker %d. %d active.", in->id, active);
        newline = 1;
    }

    if (in->logging || newline) {
        printf("\n");
    }

    fflush(NULL);

    result = (active >= SMOKERCOUNT);
    return &result;
}

int * smoker_proc_1_svc(struct smoker_info * in, struct svc_req * rqstp) {
    printf("Resources requested by smoker %d.\n", in->id);
    fflush(NULL);

    static int result;

    // Add the smoker to the active pool if necessary.
    if (!smoker_active[in->id]) {
        smoker_active[in->id] = 1;
        ++active;
    }

    _log(in);

    if (materials[in->material] > in->amount) {
        materials[in->material] -= in->amount;
        result = ENOUGH;
    }
    else {
        result = NOTENOUGH;
    }

    return &result;
}

void * smoker_exit_1_svc(struct smoker_id * in, struct svc_req * rqstp) {
    if (in->logging)
        printf("Request for Termination Received\n");

	fflush(NULL);

    if (smoker_active[in->id]) {
        smoker_active[in->id] = 0;
        --active;
    }

    if (!active) {
        printf("Request for Termination Received. Terminating.\n");
        exit(EXIT_SUCCESS);
    }
}

void _log(smoker_info * in) {
    printf("Server Receiving request from smoker %d for %d ",
            in->id,
            in->amount);
    switch (in->material) {
        case TOBACCO :
            printf("%s.\n", STR_TOBAC);
            break;
        case PAPER :
            printf("%s.\n", STR_PAPER);
            break;
        case MATCHES :
            printf("%s.\n", STR_MATCH);
            break;
        default :
            printf("%s.\n", STR_UNKNO);
            break;
    }

    printf("%d active smokers: %d %d %d\n",
            active,
            smoker_active[0],
            smoker_active[1],
            smoker_active[2]);

    fflush(NULL);
}

