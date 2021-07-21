#ifndef IM_BST_NODE_H
#define IM_BST_NODE_H

#include "IMBSTLink.h"

typedef struct IMBSTLink IMBSTLINK;

struct IMBSTNode{
	unsigned long version_stamp;
	int val;
	IMBSTLINK **links;
	struct IMBSTNode *copy;
	int num_of_used_links;
};

typedef struct IMBSTNode IMBSTNODE;

IMBSTNODE *IMBSTNODE_create(unsigned long version_stamp, int num_of_extra_links, int val, IMBSTLINK *left, IMBSTLINK *right, int memory_mapped);

#endif
