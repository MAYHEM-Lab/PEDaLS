#include <stdlib.h>

#include "IMBSTOptions.h"
#include "IMBSTNode.h"
#include "pmalloc.h"

IMBSTNODE *IMBSTNODE_create(unsigned long version_stamp, int num_of_extra_links, int val, IMBSTLINK *left, IMBSTLINK *right, int memory_mapped){

	int i;
	IMBSTNODE *node;

	if(memory_mapped){
		node = (IMBSTNODE *)Pmalloc(sizeof(IMBSTNODE));
	}else{
		node = (IMBSTNODE *)malloc(sizeof(IMBSTNODE));
	}
	SPACE += sizeof(IMBSTNODE);
	node->version_stamp = version_stamp;
	node->val = val;

	if(memory_mapped){
		node->links = (IMBSTLINK **)Pmalloc((num_of_extra_links + 2) * sizeof(IMBSTLINK *));
	}else{
		node->links = (IMBSTLINK **)malloc((num_of_extra_links + 2) * sizeof(IMBSTLINK *));
	}
	SPACE += (2 * sizeof(IMBSTLINK *));

	for(i = 0; i < num_of_extra_links + 2; ++i){
		node->links[i] = NULL;
	}
	node->links[0] = left;
	node->links[1] = right;
	node->num_of_used_links = 2;
	node->copy = NULL;

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)node->links, 2 * sizeof(IMBSTLINK *));
		PmallocSyncObject((unsigned char *)node, sizeof(IMBSTNODE));
	}

	return node;

}
