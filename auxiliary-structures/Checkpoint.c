#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Checkpoint.h"

char *CPR_str(CPR cpr){

	char *retval;
	int retsize;

	retsize = CPR_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	sprintf(retval, "WooFName:%s latest_seq_no:%lu ", 
		cpr.WooFName, cpr.latest_seq_no);

	return retval;

}

char *CP_str(CP cp){

	char *retval;
	char *temp;
	int retsize;
	int i;

	retsize = CPR_PER_CP * CPR_STR_SIZE + CP_STR_SIZE_EXTRA;
	retval = (char *)malloc(retsize * sizeof(char));

	temp = VS_str(cp.vs);
	sprintf(retval, "%s", temp);
	free(temp);

	for(i = 0; i < CPR_PER_CP; ++i){
		if(strcmp(cp.cpr[i].WooFName, "") != 0){
			temp = CPR_str(cp.cpr[i]);
			strcat(retval, temp);
			free(temp);
		}
	}

	return retval;

}
