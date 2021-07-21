#include <stdlib.h>

#include "IMLLNode.h"
#include "pmalloc.h"
#include "IMLLOptions.h"

IMLLNODE *IMLLNODE_create(unsigned long version_stamp, int num_of_extra_links, int val, IMLLLINK *link, IMLLNODE *parent, int memory_mapped){

	int i;
	IMLLNODE *node;

	if(memory_mapped){
		node = (IMLLNODE *)Pmalloc(sizeof(IMLLNODE));
	}else{
		node = (IMLLNODE *)malloc(sizeof(IMLLNODE));
	}
	SPACE += sizeof(IMLLNODE);

	node->version_stamp = version_stamp;
	node->val = val;
	if(memory_mapped){
		node->links = (IMLLLINK **)Pmalloc((num_of_extra_links) * sizeof(IMLLLINK *));
	}else{
		node->links = (IMLLLINK **)malloc((num_of_extra_links) * sizeof(IMLLLINK *));
	}
	for(i = 1; i <= num_of_extra_links; ++i){
		node->links[i] = NULL;
	}
	node->links[0] = link;
	node->num_of_used_links = 1;
	node->parent = parent;
	node->copy = NULL;

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)node, sizeof(IMLLNODE));
		PmallocSyncObject((unsigned char *)node->links, sizeof(IMLLLINK *));
	}

	return node;

}
