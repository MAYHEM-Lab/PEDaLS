#ifndef PROGRESS_STATUS
#define PROGRESS_STATUS

#define PROGSTAT_START 0
#define PROGSTAT_END 1
#define PROGSTAT_STR_SIZE 20

struct ProgressStatus{
	unsigned long remseq_idx;
	int status;
};

typedef struct ProgressStatus PROGSTAT;

char *PROGSTAT_str(PROGSTAT ps);

#endif
