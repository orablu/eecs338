struct smoker_info {
    int material;
	int amount;
    int id;
    int done;
};

struct smoker_id {
    int id;
};

program SMOKER_PROG
{
	version SMOKER_VERS
	{
        int  SMOKER_START(struct smoker_id)  = 1;
		int  SMOKER_PROC(struct smoker_info) = 2;
		void SMOKER_EXIT(struct smoker_id)   = 3;
	} = 1;
} = 0x1059860;
