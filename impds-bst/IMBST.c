#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IMBST.h"
#include "pmalloc.h"

IMBST *IMBST_init(int num_of_extra_links, int memory_mapped){

	IMBST *imbst;

	SPACE = 0;

	if(memory_mapped){
		imbst = (IMBST *)Pmalloc(sizeof(IMBST));
	}else{
		imbst = (IMBST *)malloc(sizeof(IMBST));
	}
	SPACE += sizeof(IMBST);
	imbst->memory_mapped = memory_mapped;
	imbst->vs = 0;
	imbst->num_of_extra_links = num_of_extra_links;
	memset(imbst->ap, 0, (MAX_VS + 1) * sizeof(IMBSTNODE *));

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)imbst, sizeof(IMBST));
	}

	return imbst;

}

void IMBST_populate_current_links(IMBST *imbst, IMBSTNODE *node, unsigned long vs, IMBSTLINK *left, IMBSTLINK *right){

	int i;

	memset((void *)left, 0, sizeof(IMBSTLINK));
	memset((void *)right, 0, sizeof(IMBSTLINK));

	for(i = 0; i < node->num_of_used_links; ++i){
		if(node->links[i]->type == LEFT && node->links[i]->version_stamp <= vs){
			memcpy((void *)left, (void *)node->links[i], sizeof(IMBSTLINK));
		}else if(node->links[i]->type == RIGHT && node->links[i]->version_stamp <= vs){
			memcpy((void *)right, (void *)node->links[i], sizeof(IMBSTLINK));
		}
	}

}

IMBSTNODE *IMBST_get_terminal_node(IMBST *imbst, int val, IMBSTS *stack){

	IMBSTNODE *node;
	IMBSTLINK left;
	IMBSTLINK right;
	
	node = imbst->ap[imbst->vs];
	if(node == NULL) return NULL;

	while(1){
		IMBSTS_push(stack, node);
		IMBST_populate_current_links(imbst, node, imbst->vs, &left, &right);
		if(val < node->val){
			if(left.to == NULL) return node;
			else node = left.to;
		}else{
			if(right.to == NULL) return node;
			else node = right.to;
		}
	}


	return NULL;

}

int IMBST_get_direction(IMBSTNODE *parent, IMBSTNODE *child){

	int dir;

	if(parent == NULL || child == NULL) return UNKNOWN;
	dir = (child->val < parent->val) ? LEFT : RIGHT;

	return dir;

}

IMBSTNODE *IMBST_add_node(IMBST *imbst, IMBSTNODE *parent, IMBSTNODE *child, unsigned long version_stamp, int type, int *head_changed, IMBSTS *stack){

	IMBSTLINK *link;
	IMBSTLINK *left;
	IMBSTLINK *right;
	IMBSTNODE *parent_copy;
	int num_of_used_links;
	IMBSTSN *sn;
	int new_copy_flag;
	int first_time;

	*head_changed = 0;
	new_copy_flag = 0;
	first_time = 1;

	if(parent == NULL){
		*head_changed = 1;
		return NULL;
	}

	while(1){
		parent_copy = NULL;
		sn = IMBSTS_pop(stack);
		if(sn == NULL) break;

		parent = sn->node;
		//memcpy((void *)parent, (void *)sn->node, sizeof(IMBSTNODE));
		//TODO: sn destroy

		if(! first_time){
			type = IMBST_get_direction(parent, child);
		}

		link = IMBSTLINK_create(version_stamp, child, type, imbst->memory_mapped);

		if(parent->num_of_used_links < (2 + imbst->num_of_extra_links)){// no copy
			new_copy_flag = 0;
			parent->links[parent->num_of_used_links] = link;
			parent->num_of_used_links += 1;
		}else{
			left = IMBSTLINK_create(version_stamp, child, type, imbst->memory_mapped);
			right = IMBSTLINK_create(version_stamp, child, type, imbst->memory_mapped);
			IMBST_populate_current_links(imbst, parent, version_stamp, left, right);
			parent_copy = IMBSTNODE_create(version_stamp, imbst->num_of_extra_links, parent->val, left, right, imbst->memory_mapped);
			parent->copy = parent_copy;
			new_copy_flag = 1;
			if(type == LEFT){
				parent_copy->links[0] = link;
			}else{
				parent_copy->links[1] = link;
			}
			//TODO: left/right destroy
			parent_copy->num_of_used_links = 2;
		}

		if(parent == imbst->ap[imbst->vs] && new_copy_flag){
			*head_changed = 1;
			break;
		}
		if(!new_copy_flag) break;

		child = parent_copy;
		first_time = 0;
	}

	if(parent->copy) return parent->copy;
	else return parent;

}

void IMBST_insert(IMBST *imbst, int val){

	IMBSTNODE *node;
	IMBSTNODE *terminal_node;
	IMBSTLINK *left;
	IMBSTLINK *right;
	IMBSTS *stack;
	unsigned long working_vs;
	IMBSTNODE *add_node_retval;
	int type;
	int head_changed;

	node = NULL;
	add_node_retval = NULL;
	
	working_vs = imbst->vs + 1;

	left = IMBSTLINK_create(working_vs, NULL, LEFT, imbst->memory_mapped);
	right = IMBSTLINK_create(working_vs, NULL, RIGHT, imbst->memory_mapped);
	node = IMBSTNODE_create(working_vs, imbst->num_of_extra_links, val, left, right, imbst->memory_mapped);

	if(working_vs == 1 || imbst->ap[imbst->vs] == NULL){
		imbst->ap[working_vs] = node;
		imbst->vs = working_vs;
		return;
	}

	stack = IMBSTS_init(imbst->memory_mapped);
	terminal_node = IMBST_get_terminal_node(imbst, val, stack);
	type = IMBST_get_direction(terminal_node, node);
	add_node_retval = IMBST_add_node(imbst, terminal_node, node, working_vs, type, &head_changed, stack);

	if(add_node_retval == NULL){
		imbst->ap[working_vs] = node;
	}if(head_changed){
		imbst->ap[working_vs] = add_node_retval;
	}else{
		imbst->ap[working_vs] = imbst->ap[imbst->vs];
	}

	imbst->vs = working_vs;

}

IMBSTNODE *IMBST_search(IMBST *imbst, int val, unsigned long vs, IMBSTS *stack){

	IMBSTNODE *node;
	IMBSTLINK left;
	IMBSTLINK right;

	node = imbst->ap[vs];

	while(node != NULL){
		IMBSTS_push(stack, node);
		if(val == node->val) return node;
		IMBST_populate_current_links(imbst, node, vs, &left, &right);
		if(val < node->val) node = left.to;
		else node = right.to;
	}

	return NULL;

}

IMBSTNODE *IMBST_get_predecessor(IMBST *imbst, IMBSTNODE *node, IMBSTS *stack){

	IMBSTLINK left;
	IMBSTLINK right;
	IMBSTNODE *pred;

	IMBST_populate_current_links(imbst, node, imbst->vs, &left, &right);
	pred = left.to;

	while(1){
		IMBSTS_push(stack, pred);
		IMBST_populate_current_links(imbst, pred, imbst->vs, &left, &right);
		if(right.to == NULL) break;
		pred = right.to;
	}

	return pred;

}

void IMBST_delete(IMBST *imbst, int val){

	IMBSTS *stack;
	IMBSTS *pred_stack;
	IMBSTS *auxiliary_stack;
	IMBSTNODE *target;
	IMBSTSN *popped;
	IMBSTNODE *parent;
	IMBSTNODE *pred;
	IMBSTNODE *pred_parent;
	IMBSTLINK *left;
	IMBSTLINK *right;
	IMBSTLINK *pred_left;
	IMBSTLINK *pred_right;
	unsigned long working_vs;
	int type;
	int head_changed;
	IMBSTNODE *add_node_retval;

	stack = NULL;
	parent = NULL;
	target = NULL;
	working_vs = imbst->vs + 1;
	type = UNKNOWN;
	head_changed = 0;

	stack = IMBSTS_init(imbst->memory_mapped);
	target = IMBST_search(imbst, val, imbst->vs, stack);
	if(target == NULL) return;

	popped = IMBSTS_pop(stack);
	popped = IMBSTS_pop(stack);
	if(popped){
		parent = popped->node;
		IMBSTS_push(stack, popped->node);
	}

	left = IMBSTLINK_create(0, NULL, 0, imbst->memory_mapped);
	right = IMBSTLINK_create(0, NULL, 0, imbst->memory_mapped);
	IMBST_populate_current_links(imbst, target, working_vs, left, right);

	if(left->to == NULL && right->to == NULL){
		if(imbst->ap[imbst->vs] == target){
			head_changed = 1;
			add_node_retval = NULL;
		}else{
			type = IMBST_get_direction(parent, target);
			add_node_retval = IMBST_add_node(imbst, parent, NULL, working_vs, type, &head_changed, stack);
		}
	}else if(left->to != NULL && right->to == NULL){
		if(imbst->ap[imbst->vs] == target){
			head_changed = 1;
			add_node_retval = left->to;
		}else{
			type = IMBST_get_direction(parent, target);
			add_node_retval = IMBST_add_node(imbst, parent, left->to, working_vs, type, &head_changed, stack);
		}
	}else if(left->to == NULL && right->to != NULL){
		if(imbst->ap[imbst->vs] == target){
			head_changed = 1;
			add_node_retval = right->to;
		}else{
			type = IMBST_get_direction(parent, target);
			add_node_retval = IMBST_add_node(imbst, parent, right->to, working_vs, type, &head_changed, stack);
		}
	}else{
		pred_stack = IMBSTS_init(imbst->memory_mapped);
		pred = IMBST_get_predecessor(imbst, target, pred_stack);
		if(pred == left->to){
			//step 1: add (target->right) to (pred)
			auxiliary_stack = IMBSTS_init(imbst->memory_mapped);
			IMBSTS_push(auxiliary_stack, pred);
			add_node_retval = IMBST_add_node(imbst, pred, right->to, working_vs, RIGHT, &head_changed, auxiliary_stack);
			if(add_node_retval != NULL) pred = add_node_retval;
			//step 2: add (pred) to (parent)
			if(parent != NULL){
				type = IMBST_get_direction(parent, pred);
				add_node_retval = IMBST_add_node(imbst, parent, pred, working_vs, type, &head_changed, stack);
			}else{
				head_changed = 1;
			}
		}else{
			pred_left = IMBSTLINK_create(0, NULL, 0, imbst->memory_mapped);
			pred_right = IMBSTLINK_create(0, NULL, 0, imbst->memory_mapped);
			IMBST_populate_current_links(imbst, pred, working_vs, pred_left, pred_right);

			popped = IMBSTS_pop(pred_stack);
			popped = IMBSTS_pop(pred_stack);
			if(popped){
				pred_parent = popped->node;
				IMBSTS_push(pred_stack, popped->node);
			}

			//step 1: add (pred->left) to (pred->parent)
			//notice both (pred->parent) and (target->left) might change
			add_node_retval = IMBST_add_node(imbst, pred_parent, pred_left->to, working_vs, RIGHT, &head_changed, pred_stack);
			if(pred_parent->copy){
				pred_parent = pred_parent->copy;
			}
			if(left->to->copy){
				left->to = left->to->copy;
			}
			//step 2a: add (target->left) to (pred)
			auxiliary_stack = IMBSTS_init(imbst->memory_mapped);
			IMBSTS_push(auxiliary_stack, pred);
			head_changed = 0;
			add_node_retval = IMBST_add_node(imbst, pred, left->to, working_vs, LEFT, &head_changed, auxiliary_stack);
			if(pred->copy) pred = pred->copy;
			//step 2b: add (target->right) to (pred)
			auxiliary_stack = IMBSTS_init(imbst->memory_mapped);
			IMBSTS_push(auxiliary_stack, pred);
			head_changed = 0;
			add_node_retval = IMBST_add_node(imbst, pred, right->to, working_vs, RIGHT, &head_changed, auxiliary_stack);
			if(pred->copy) pred = pred->copy;
			//step 3: add (pred) to (target->parent)
			if(parent){
				type = IMBST_get_direction(parent, pred);
				head_changed = 0;
				add_node_retval = IMBST_add_node(imbst, parent, pred, working_vs, type, &head_changed, stack);
			}else{
				head_changed = 1;
			}
		}
	}

	if(head_changed){
		imbst->ap[working_vs] = add_node_retval;
	}else{
		imbst->ap[working_vs] = imbst->ap[imbst->vs];
	}

	imbst->vs = working_vs;

}

void IMBST_print_preorder(IMBST *imbst, IMBSTNODE *node, unsigned long vs){

	IMBSTLINK left;
	IMBSTLINK right;

	if(node == NULL) return;

	IMBST_populate_current_links(imbst, node, vs, &left, &right);

	fprintf(stdout, "%d ", node->val);
	IMBST_print_preorder(imbst, left.to, vs);
	IMBST_print_preorder(imbst, right.to, vs);

}

void IMBST_preorder(IMBST *imbst, unsigned long vs){

	IMBSTNODE *node;

	node = imbst->ap[vs];
	IMBST_print_preorder(imbst, node, vs);

	fprintf(stdout, "\n");

}

void IMBST_traverse_max(IMBST *imbst, int *num){

	IMBSTNODE *node;
	int i;

	*num = 0;

	if(imbst == NULL) return;
	node = imbst->ap[imbst->vs];

	while(node != NULL){
		*num += 1;
		for(i = node->num_of_used_links - 1; i >= 0; --i){
			if(node->links[i]->type == RIGHT){
				node = node->links[i]->to;
				break;
			}
		}
	}

}
