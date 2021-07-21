#ifndef IMLL_NODE_H
#define IMLL_NODE_H

#include "IMLLLink.h"

typedef struct IMLLLink IMLLLINK;

struct IMLLNode{
	unsigned long version_stamp;
	int val;
	IMLLLINK **links;
	struct IMLLNode *parent;
	struct IMLLNode *copy;
	int num_of_used_links;
};

typedef struct IMLLNode IMLLNODE;

IMLLNODE *IMLLNODE_create(unsigned long version_stamp, int num_of_extra_links, int val, IMLLLINK *link, IMLLNODE *parent, int memory_mapped);

#endif
