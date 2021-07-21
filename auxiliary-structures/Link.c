#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Link.h"

char *LINK_str(LINK link){

	char *retval;
	char *vs_retval;
	char *dw_vs_retval;
	char *node_retval;
	int retsize;

	retsize = 120;
	retval = (char *)malloc(retsize * sizeof(char));

	vs_retval = VS_str(link.vs);
	dw_vs_retval = VS_str(link.dw_vs);
	node_retval = NODE_str(link.node);
	sprintf(retval, "%s%s%s rem:%d type:%c",
		vs_retval, dw_vs_retval, node_retval, 
		link.num_of_remaining_links, link.type);
	free(vs_retval);
	free(dw_vs_retval);
	free(node_retval);

	return retval;

}

