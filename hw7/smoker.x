struct smoker_info {
    int material;
	int amount;
    int id;
    int done;
};

struct exit_info {
    int id;
};

program SMOKER_PROG
{
	version SMOKER_VERS
	{
		void SMOKER_PROC(struct smoker_info) = 1;
		void SMOKER_EXIT(struct exit_info) = 2;
	} = 1;
} = 0x1059860;
