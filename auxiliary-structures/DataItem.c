#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DataItem.h"

char *DI_str(DI di){
	
	char *retval;
	int retsize;

	retsize = DI_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	sprintf(retval, "%d ", di.val);

	return retval;

}

int DI_cmp(DI di1, DI di2){
	if(di1.val == di2.val) return 0;
	else if(di1.val < di2.val) return -1;
	else return 1;
}
