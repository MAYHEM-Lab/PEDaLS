#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Knowledge.h"

char *KNOW_str(KNOW k){

	char *vs_str;
	char *retval;
	int retsize;

	retsize = KNOW_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	vs_str = VS_str(k.vs);
	sprintf(retval, "%s seq_no:%lu ", vs_str, k.seq_no);

	free(vs_str);

	return retval;

}

char *KNOW_get_k_woof_name_from_VS(VS vs){
	
	char *k_woof_name;
	int retsize;

	retsize = 2;
	k_woof_name = (char *)malloc(retsize * sizeof(char));
	memset(k_woof_name, 0, retsize);
	k_woof_name[0] = vs.nodeID;

	return k_woof_name;

}
