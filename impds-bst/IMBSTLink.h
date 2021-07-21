#ifndef IM_BST_LINK_H
#define IM_BST_LINK_H

#include "IMBSTNode.h"

typedef struct IMBSTNode IMBSTNODE;

struct IMBSTLink{
	unsigned long version_stamp;
	int type;
	IMBSTNODE *to;
};

typedef struct IMBSTLink IMBSTLINK;

IMBSTLINK *IMBSTLINK_create(unsigned long version_stamp, IMBSTNODE *to, int type, int memory_mapped);

#endif
