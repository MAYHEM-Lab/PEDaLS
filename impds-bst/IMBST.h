#ifndef IM_BST_H
#define IM_BST_H

#include "IMBSTNode.h"
#include "IMBSTStack.h"
#include "IMBSTOptions.h"

struct IMBST{
	unsigned long vs;
	IMBSTNODE *ap[MAX_VS + 1];
	int num_of_extra_links;
	int memory_mapped;
};

typedef struct IMBST IMBST;

IMBST *IMBST_init(int num_of_extra_links, int memory_mapped);
void IMBST_populate_current_links(IMBST *imbst, IMBSTNODE *node, unsigned long vs, IMBSTLINK *left, IMBSTLINK *right);
IMBSTNODE *IMBST_get_terminal_node(IMBST *imbst, int val, IMBSTS *stack);
int IMBST_get_direction(IMBSTNODE *parent, IMBSTNODE *child);
IMBSTNODE *IMBST_add_node(IMBST *imbst, IMBSTNODE *parent, IMBSTNODE *child, unsigned long version_stamp, int type, int *head_changed, IMBSTS *stack);
void IMBST_insert(IMBST *imbst, int val);
IMBSTNODE *IMBST_search(IMBST *imbst, int val, unsigned long vs, IMBSTS *stack);
IMBSTNODE *IMBST_get_predecessor(IMBST *imbst, IMBSTNODE *node, IMBSTS *stack);
void IMBST_delete(IMBST *imbst, int val);
void IMBST_preorder(IMBST *imbst, unsigned long vs);
void IMBST_traverse_max(IMBST *imbst, int *num);

#endif
