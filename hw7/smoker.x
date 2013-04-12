struct smoker_info {
	int one;
	int two;
};

program SMOKER_PROG
{
	version SMOKER_VERS
	{
		int SMOKER_PROC(struct smoker_info) = 1;
		void SMOKER_EXIT(struct smoker_info) = 2;
	} = 1;
} = 0x1059860;
