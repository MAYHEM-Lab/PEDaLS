#ifndef IM_BST_STACK_H
#define IM_BST_STACK_H

#include "IMBSTNode.h"

struct IMBSTStackNode{
	IMBSTNODE *node;
	struct IMBSTStackNode *next;
};

typedef struct IMBSTStackNode IMBSTSN;

struct IMBSTStack{
	IMBSTSN *head;
	int memory_mapped;
};

typedef struct IMBSTStack IMBSTS;

IMBSTS *IMBSTS_init(int memory_mapped);
void IMBSTS_push(IMBSTS *stack, IMBSTNODE *node);
IMBSTSN *IMBSTS_pop(IMBSTS *stack);
#endif
