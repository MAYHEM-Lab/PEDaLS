#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Options.h"
#include "VersionStamp.h"

int VS_node_id_cmp(VS a, VS b){
	if(a.nodeID < b.nodeID) return -1;
	else if(a.nodeID > b.nodeID) return 1;
	return 0;
}

int VS_cmp(VS a, VS b){
	if(a.counter < b.counter){
		return -1;
	}else if(a.counter > b.counter){
		return 1;
	}else{
		return VS_node_id_cmp(a, b);
	}
}

char *VS_str(VS vs){

	char *retval;
	int retsize;

	retsize = VS_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	sprintf(retval, "%lu%c ", vs.counter, vs.nodeID);

	return retval;

}

VS VS_invalid(){

	VS vs;

	vs.counter = INVALID_COUNTER;
	vs.nodeID = INVALID_NODE_ID;

	return vs;

}
