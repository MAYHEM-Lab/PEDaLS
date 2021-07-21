#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Node.h"

char *NODE_str(NODE node){

	char *retval;
	int retsize;

	retsize = NODE_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));
	sprintf(retval, "dw_seq_no:%lu lw_seq_no:%lu ",
		node.dw_seq_no, node.lw_seq_no);

	return retval;

}

NODE NODE_failed_op(){
	
	NODE node;

	node.dw_seq_no = (unsigned long)-1;
	node.lw_seq_no = (unsigned long)-1;

	return node;

}

bool NODE_is_equal(NODE a, NODE b){
	
	return ((a.dw_seq_no == b.dw_seq_no) && (a.lw_seq_no == b.lw_seq_no));

}
