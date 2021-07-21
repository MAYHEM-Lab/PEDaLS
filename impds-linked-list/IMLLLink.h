#ifndef IMLL_LINK_H
#define IMLL_LINK_H

#include "IMLLNode.h"

typedef struct IMLLNode IMLLNODE;

struct IMLLLink{
	unsigned long version_stamp;
	IMLLNODE *to;
};

typedef struct IMLLLink IMLLLINK;

IMLLLINK *IMLLLINK_create(unsigned long version_stamp, IMLLNODE *to, int memory_mapped);

#endif
