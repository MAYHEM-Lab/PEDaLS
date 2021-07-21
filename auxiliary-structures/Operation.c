#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Operation.h"

char *OP_str(OP op){

	char *retval;
	char *vs_retval;
	char *di_retval;
	int retsize;

	retsize = OP_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	vs_retval = VS_str(op.vs);
	di_retval = DI_str(op.di);
	sprintf(retval, "%s%s op:%d", vs_retval, di_retval, op.op);

	free(vs_retval);
	free(di_retval);

	return retval;

}
