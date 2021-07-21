#ifndef IM_LINKED_LIST_H
#define IM_LINKED_LIST_H

#include "IMLLNode.h"
#include "IMLLOptions.h"

struct IMLL{
	unsigned long vs;
	IMLLNODE *ap[MAX_VS + 1];
	int num_of_extra_links;
	int memory_mapped;
};

typedef struct IMLL IMLL;

IMLL *IMLL_init(int num_of_extra_links, int memory_mapped);
IMLLNODE *IMLL_get_terminal_node(IMLL *);
void IMLL_add_node(IMLL *imll, IMLLNODE *parent, IMLLNODE *child, unsigned long working_vs);
void IMLL_insert(IMLL *imll, int val);
IMLLNODE *IMLL_get_node_at_link(IMLL *imll, IMLLNODE *node, unsigned long version_stamp);
void IMLL_print(IMLL *imll, unsigned long version_stamp);
IMLLNODE *IMLL_search(IMLL *imll, unsigned long version_stamp, int val);
void IMLL_delete(IMLL *imll, int val);
void IMLL_destroy(IMLL *imll);
void IMLL_traverse(IMLL *imll, int *num);

#endif
