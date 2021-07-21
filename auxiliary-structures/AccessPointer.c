#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AccessPointer.h"

char *AP_str(AP ap){

	char *retval;
	char *vs_retval;
	char *node_retval;
	int retsize;

	retsize = AP_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	vs_retval = VS_str(ap.vs);
	node_retval = NODE_str(ap.node);
	sprintf(retval, "%s%s", vs_retval, node_retval);
	free(vs_retval);
	free(node_retval);

	return retval;

}
