#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Data.h"

char *DATA_str(DATA data){

	char *retval;
	char *vs_retval;
	char *di_retval;
	int retsize;

	retsize = DATA_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	vs_retval = VS_str(data.vs);
	di_retval = DI_str(data.di);
	sprintf(retval, "%s%slw_name:%s", vs_retval, di_retval, data.lw_name);

	free(vs_retval);
	free(di_retval);

	return retval;

}
