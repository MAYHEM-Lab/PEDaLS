#include <stdio.h>
#include <stdlib.h>

#include "ProgressStatus.h"

char *PROGSTAT_str(PROGSTAT ps){

	char *retval;
	int retsize;

	retsize = PROGSTAT_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));
	sprintf(retval, "%lu %d ", ps.remseq_idx, ps.status);

	return retval;

}
