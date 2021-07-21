#include <stdio.h>
#include <stdlib.h>

#include "TopologyNode.h"

char *TOPNODE_str(TOPNODE tn){
	char *retval;
	int retsize;

	retsize = TOPNODE_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	sprintf(retval, "nodeID:%c IP:%s ", tn.nodeID, tn.IP);
	return retval;
}
