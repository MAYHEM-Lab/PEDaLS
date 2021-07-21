#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IMLL.h"
#include "IMLLOptions.h"
#include "pmalloc.h"

IMLL *IMLL_init(int num_of_extra_links, int memory_mapped){

	IMLL *imll;

	if(memory_mapped){
		imll = (IMLL *)Pmalloc(sizeof(IMLL));
	}else{
		imll = (IMLL *)malloc(sizeof(IMLL));
	}
	SPACE += sizeof(IMLL);

	imll->memory_mapped = memory_mapped;
	imll->vs = 0;
	imll->num_of_extra_links = num_of_extra_links;
	memset(imll->ap, 0, (MAX_VS + 1) * sizeof(IMLLNODE *));

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)imll, sizeof(IMLL));
	}

	return imll;
}

IMLLNODE *IMLL_get_terminal_node(IMLL *imll){

	IMLLNODE *node;
	int i;
	int max_vs_index;
	unsigned long max_vs;
	unsigned long candidate_max_vs;
	int num_of_used_links;

	node = imll->ap[imll->vs];

	while(1){
		max_vs_index = 0;
		max_vs = 0;
		num_of_used_links = node->num_of_used_links;
		for(i = 0; i < num_of_used_links; ++i){
			candidate_max_vs = node->links[i]->version_stamp;
			if(candidate_max_vs > max_vs){
				max_vs = candidate_max_vs;
				max_vs_index = i;
			}
		}
		if(node->links[max_vs_index]->to == NULL){
			return node;
		}else{
			node = node->links[max_vs_index]->to;
		}
	}

	return NULL;

}

void IMLL_add_node(IMLL *imll, IMLLNODE *parent, IMLLNODE *child, unsigned long version_stamp){

	IMLLLINK *link;
	IMLLNODE *parent_copy;
	int num_of_used_links;

	link = IMLLLINK_create(version_stamp, child, imll->memory_mapped);

	num_of_used_links = parent->num_of_used_links;
	if(num_of_used_links < (imll->num_of_extra_links + 1)){
		parent->links[num_of_used_links] = link;
		parent->num_of_used_links = num_of_used_links + 1;
		if(child){
			child->parent = parent;
		}
		return;
	}

	parent_copy = IMLLNODE_create(version_stamp, imll->num_of_extra_links, parent->val, link, parent->parent, imll->memory_mapped);
	parent->copy = parent_copy;
	if(child) {
		child->parent = parent_copy;
	}

	if(parent == imll->ap[imll->vs]){
		imll->ap[version_stamp] = parent_copy;
	}else{
		IMLL_add_node(imll, parent->parent, parent_copy, version_stamp);
	}

}

void IMLL_insert(IMLL *imll, int val){
	
	IMLLNODE *node;
	IMLLNODE *terminal_node;
	IMLLLINK *link;
	unsigned long working_vs;
	
	node = NULL;
	
	working_vs = imll->vs + 1;

	link = IMLLLINK_create(working_vs, NULL, imll->memory_mapped);
	node = IMLLNODE_create(working_vs, imll->num_of_extra_links, val, link, NULL, imll->memory_mapped);

	if(working_vs == 1 || imll->ap[imll->vs] == NULL){
		imll->ap[working_vs] = node;
		imll->vs = working_vs;
		return;
	}

	terminal_node = IMLL_get_terminal_node(imll);
	IMLL_add_node(imll, terminal_node, node, working_vs);

	if(imll->ap[working_vs] == NULL){
		imll->ap[working_vs] = imll->ap[imll->vs];
	}

	imll->vs = working_vs;

}

IMLLNODE *IMLL_get_node_at_link(IMLL *imll, IMLLNODE *node, unsigned long version_stamp){

	unsigned long max_vs;
	int max_vs_index;
	int i;

	if(node == NULL) return NULL;

	max_vs = 0;
	max_vs_index = 0;
	for(i = 0; i < node->num_of_used_links; ++i){
		if(node->links[i]->version_stamp <= version_stamp && node->links[i]->version_stamp >= max_vs){
			max_vs = node->links[i]->version_stamp;
			max_vs_index = i;
		}
	}
	return node->links[max_vs_index]->to;

}

void IMLL_print(IMLL *imll, unsigned long version_stamp){
	
	IMLLNODE *node;

	node = imll->ap[version_stamp];
	while(node != NULL){
		printf("%d ", node->val);
		node = IMLL_get_node_at_link(imll, node, version_stamp);
	}
	fprintf(stdout, "\n");
	fflush(stdout);

}

IMLLNODE *IMLL_search(IMLL *imll, unsigned long version_stamp, int val){

	IMLLNODE *node;

	node = imll->ap[version_stamp];

	while(node){
		if(node->val == val){
			return node;
		}
		node = IMLL_get_node_at_link(imll, node, version_stamp);
	}

	return NULL;

}

void IMLL_delete(IMLL *imll, int val){

	IMLLNODE *node;
	IMLLNODE *parent;
	IMLLNODE *child;
	unsigned long working_vs;

	working_vs = imll->vs + 1;

	node = NULL;
	node = IMLL_search(imll, imll->vs, val);
	if(node == NULL){
		return;
	}

	if(node == imll->ap[imll->vs]){
		imll->ap[working_vs] = IMLL_get_node_at_link(imll, node, imll->vs);
		imll->vs = working_vs;
		return;
	}

	parent = node->parent;
	child = IMLL_get_node_at_link(imll, node, imll->vs);

	IMLL_add_node(imll, parent, child, working_vs);

	if(imll->ap[working_vs] == NULL){
		imll->ap[working_vs] = imll->ap[imll->vs];
	}

	imll->vs = working_vs;

}

void IMLL_destroy(IMLL *imll){

	free(imll);

}

void IMLL_traverse(IMLL *imll, int *num){

	IMLLNODE *node;

	if(imll == NULL) return;
	node = imll->ap[imll->vs];

	while(node != NULL){
		*num += 1;
		node = node->links[node->num_of_used_links - 1]->to;
	}

}
