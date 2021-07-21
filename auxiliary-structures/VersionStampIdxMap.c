#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "VersionStampIdxMap.h"

char *VSIDX_str(VSIDX vsidx){

	char *retval;
	char *vs_retval;
	int retsize;

	retsize = VSIDX_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));
	memset((void *)retval, 0, retsize);

	vs_retval = VS_str(vsidx.vs);
	sprintf(retval, "%s idx:%lu", vs_retval, vsidx.idx);
	free(vs_retval);

	return retval;

}
