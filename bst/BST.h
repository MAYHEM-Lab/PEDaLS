#ifndef BST_H
#define BST_H

#include <stdbool.h>

#include "AccessPointer.h"
#include "Accumulator.h"
#include "Config.h"
#include "DataItem.h"
#include "HashTable.h"
#include "Link.h"
#include "Options.h"
#include "Stack.h"
#include "VersionStamp.h"

extern CONFIG config;
extern HT *cp_ht;

/**
 * initializes the bst
 *
 * num_of_extra_links: number of extra links per node for persistent version, 0 indicates ephemeral
 * data_woof_size: history size of data woof
 * link_woof_size: history size of link woof
 * ap_woof_size: history size of ap woof
 *
 * returns 1 if initialization successful, 0 otherwise
 **/
int BST_init(
	int num_of_extra_links,
	unsigned long data_woof_size,
	unsigned long link_woof_size,
	unsigned long ap_woof_size
);

/**
 * populates path from root to search target
 *
 * @param di: target data item
 * @param vs: vs to search in
 * @param node: out param, node containing target
 * @param stack: path from root to node gets populated here
 *
 * returns 1 on success, 0 on failure, not found indicated through node
 **/
int search(DI di, VS vs, NODE *node, Stack *stack);

/**
 * searches for data item in a given version (persistent) or in the latest version (ephemeral)
 *
 * di: data item to be searched
 * vs: version at which di is searched (ignored for ephemeral)
 *
 * returns the seq. no. in data woof of di if found, 0 if not found, unsigned long representation of -1 if op fails
 **/
unsigned long BST_search(DI di, VS vs);

/**
 * inserts data item into the bst
 *
 * di: data item to be inserted
 *
 * returns new vs if insertion successful, invalid vs otherwise
 **/
VS BST_insert(DI di, VS vs);

/**
 * deletes data item from the bst
 *
 * di: data item to be deleted
 *
 * returns new vs if deletion successful, invalid vs otherwise
 **/
VS BST_delete(DI di, VS vs);

/**
 * populates predecessor of target node
 *
 * @param vs: the version stamp for which to populate predecessor
 * @param target: target node
 * @param pred: out param, predecessor of target gets populated here
 * @param pred_stack: all nodes from left child of target to predecessor get populated here
 **/
int populate_predecessor(VS vs, NODE target, NODE *pred, Stack *pred_stack);

/**
 * accumulates preorder list in acc
 *
 * @param vs: version stamp at which traversal takes place
 * @param node: current node in recursion
 * @param acc: out param, preorder list gets accumulated
 *
 * returns 1 on success, 0 on failure
 **/
int accumulate_preorder(VS vs, NODE node, Accumulator *acc);

/**
 * accumulates preorder list of bst
 *
 * @param vs: vs at which preorder list is populated
 *
 * returns accumulator containing preorder list
 **/
Accumulator *preorder_retrieve(VS vs);

/**
 * prints the bst at a given version (persistent) or the latest version (ephemeral)
 *
 * vs: the version which is printed (ignored for ephemeral)
 *
 * returns 1 if successful, 0 otherwise
 **/
int BST_preorder(VS vs);

/**************************************************************/
/* Helper functions a client would not interact with directly */
/**************************************************************/
/**
 * populates latest AP
 * returns 1 on success, 0 on failure
 **/
int populate_latest_AP(AP *ap);

/**
 * populates working VS
 * 
 * @param vs: out param to be populated
 * @param latest_vs: latest vs
 * @param assigned_vs: assign this vs to vs if valid
 *
 * returns 1 on success, 0 on failure
 **/
int populate_working_VS(VS *vs, VS latest_vs, VS assigned_vs);

/**
 * populates config
 * @param reload: reload if true
 * returns 1 on success, 0 on failure
 **/
int load_config(bool reload);

/**
 * @param vs: vs whose order is obtained
 * @param failed: out param, true if op failed
 *
 * returns position of VS in global AP on success, ULONG_MAX if cannot be determined and vs counter is greater than the counter of the last entry in ap map (for replication), out param failed set to true on failure
 **/
unsigned long get_VS_order(VS vs, bool *failed);

/**
 * @param vs: vs for which current link will be populated
 * @param node: node for which current link will be populated
 * @param left: out param, populated left link
 * @param right: out param, populated right link
 *
 * returns 1 on success, 0 on failure
 **/
int populate_current_links(VS vs, NODE node, LINK *left, LINK *right);

/**
 * @param di: di to be inserted next to terminal node
 * @param terminal_node: out param, terminal node gets populated
 * @param stack: out param, stores nodes along the path from root to terminal
 *
 * returns 1 on success, 0 on failure
 **/
int populate_terminal_node(DI di, AP root_ap, NODE *terminal_node, Stack *stack);

/**
 * adds new node to the terminal node
 *
 * @param working_vs: working vs
 * @param terminal_node: terminal node
 * @param new_node: new node
 * @param stack: stack pointer, all nodes from root to terminal is in stack
 * @param type: left or right (intermediate steps in deletion), 'N' if not known yet (for insertion)
 * @param current_head: current head of the structure
 * @param head_changed: out param, set to true if current head is changed
 *
 * returns (0, 0) if added to empty structure,
 * 		   (x, x) x = unsigned long representation of -1 on failure,
 *		   (dw_seq_no, lw_seq_no) of the latest terminal node (might change from initial terminal node due to copy)
 **/
NODE add_node(VS working_vs, NODE terminal_node, NODE new_node, Stack *stack, char type, NODE current_head, bool *head_changed);

/**
 * populates AP corresponding to the given VS
 *
 * @param vs: version stamp for which AP will be populated
 * @param ap: out param, ap gets populated
 * 
 * returns 1 on success, 0 on failure
 **/
int populate_AP(VS vs, AP *ap);

/**
 * returns the lw_seq_no corresponding to latest copy of node
 *
 * @param node: node for which lw_seq_no will be returned
 *
 * returns lw_seq_no on success, (unsigned long)-1 on failure
 **/
unsigned long get_latest_lw_seq_no(NODE node);

/**
 * dumps debug info
 **/
void BST_debug();

void BST_traverse_max(int *num);

#endif
