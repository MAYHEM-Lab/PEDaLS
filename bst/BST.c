#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Accumulator.h"
#include "BST.h"
#include "Checkpoint.h"
#include "Config.h"
#include "CSPOTHelpers.h"
#include "Data.h"
#include "GenericHelpers.h"
#include "Knowledge.h"
#include "Operation.h"
#include "ReplicationHelpers.h"
#include "Sequencer.h"
#include "Topology.h"
#include "VersionStamp.h"
#include "VersionStampIdxMap.h"

#include "woofc-access.h"

CONFIG config;
HT *cp_ht;

int BST_init(
	int num_of_extra_links,
	unsigned long data_woof_size,
	unsigned long link_woof_size,
	unsigned long ap_woof_size
){

	int status;
	unsigned long idx;

	cp_ht = NULL;

	config.nodeID = 'A';
	if(config.nodeID == 0) return 0;
	config.num_of_extra_links = num_of_extra_links;
	config.num_of_links = 2 + num_of_extra_links;			// for bst default 2
	config.data_woof_size = data_woof_size;
	config.link_woof_size = link_woof_size;
	config.ap_woof_size = ap_woof_size;
	config.config_woof_size = CONFIG_WOOF_SIZE;
	config.checkpoint_woof_size = ap_woof_size;
	config.operation_woof_size = ap_woof_size;
	config.sequencer_woof_size = ap_woof_size;
	config.max_counter_woof_size = MAX_COUNTER_WOOF_SIZE;

	status = createWooF(DATA_WOOF_NAME, sizeof(DATA), config.data_woof_size);
	if(status < 0) return 0;
	status = createWooF(AP_WOOF_NAME, sizeof(AP), config.ap_woof_size);
	if(status < 0) return 0;
	status = createWooF(CONFIG_WOOF_NAME, sizeof(AP), config.config_woof_size);
	if(status < 0) return 0;
	status = createWooF(OPERATION_WOOF_NAME, sizeof(OP), config.operation_woof_size);
	if(status < 0) return 0;
	status = createWooF(SEQUENCER_WOOF_NAME, sizeof(SEQ), config.sequencer_woof_size);
	if(status < 0) return 0;

	idx = insertIntoWooF(CONFIG_WOOF_NAME, NULL, (void *)&config, NULL);
	if(idx == (unsigned long)-1) return 0;

	return 1; // successfully initialized

}

int search(DI di, VS vs, NODE *node, Stack *stack){

	int status;
	AP head;
	DATA data;
	LINK left;
	LINK right;
	NODE iterator;

	status = load_config(false);
	if(status == 0) return 0;
	memset((void *)node, 0, sizeof(NODE));

	status = (config.num_of_extra_links == 0 || (VS_cmp(VS_invalid(), vs) == 0)) ? populate_latest_AP(&head) : populate_AP(vs, &head);
	if(status == 0) return 0;

	memcpy((void *)&iterator, (void *)&head.node, sizeof(NODE));

	if(iterator.dw_seq_no == 0) return 1; // empty structure
	
	while(1){
		if(iterator.dw_seq_no == 0) break;
		Stack_push(stack, (void *)&iterator);
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, iterator.dw_seq_no);
		if(status < 0) return 0;
		if(DI_cmp(di, data.di) == 0){
			memcpy((void *)node, (void *)&iterator, sizeof(NODE));
			return 1;
		}
		status = populate_current_links(vs, iterator, &left, &right);
		if(status == 0) return 0;
		if(DI_cmp(di, data.di) < 0){
			memcpy((void *)&iterator, (void *)&left.node, sizeof(NODE));
		}else{
			memcpy((void *)&iterator, (void *)&right.node, sizeof(NODE));
		}
	}

	return 1; // not found

}

unsigned long BST_search(DI di, VS vs){

	NODE node;
	Stack *stack;
	int status;

	stack = Stack_init(sizeof(NODE));
	status = search(di, vs, &node, stack);
	Stack_destroy(stack);

	if(status == 0) return (unsigned long)-1;

	return node.dw_seq_no;

}

VS BST_insert(DI di, VS vs){

	AP root_ap;
	bool head_changed;
	char *lw_name;
	DATA data;
	int status;
	LINK link;
	NODE node;
	NODE terminal_node;
	NODE add_node_return;
	Stack *stack;
	unsigned long idx;
	unsigned long data_seq;
	unsigned long link_seq_first;
	unsigned long link_seq_second;
	VS working_vs;
	VS current_vs;

	/* initialize */
	status = load_config(false);
	if(status == 0) return VS_invalid();
	head_changed = false;

	status = populate_latest_AP((void *)&root_ap);
	if(status == 0) return VS_invalid();

	memcpy((void *)&current_vs, (void *)&root_ap.vs, sizeof(VS));
	status = populate_working_VS((void *)&working_vs, current_vs, vs);

	cp_ht = NULL;
	lw_name = NULL;

	/* create link woof */
	lw_name = getLinkWooFName(working_vs);
	status = createWooF(lw_name, sizeof(LINK), config.link_woof_size);
	if(status < 0) return VS_invalid();

	/* populate and insert new data entry */
	memcpy((void *)&data.vs, (void *)&working_vs, sizeof(VS));
	memcpy((void *)&data.di, (void *)&di, sizeof(DI));
	strcpy(data.lw_name, lw_name);
	data_seq = insertIntoWooF(DATA_WOOF_NAME, NULL, (void *)&data, cp_ht);
	if(data_seq == (unsigned long)-1) return VS_invalid();

	/* populate and insert new node to null pointer link */
	memset((void *)&link, 0, sizeof(LINK));
	memcpy((void *)&link.vs, (void *)&working_vs, sizeof(VS));
	memcpy((void *)&link.dw_vs, (void *)&working_vs, sizeof(VS));
	link.num_of_remaining_links = config.num_of_links - 1;
	link.type = 'L';
	link_seq_first = insertIntoWooF(data.lw_name, NULL, (void *)&link, cp_ht);
	if(link_seq_first == (unsigned long)-1) return VS_invalid();
	link.num_of_remaining_links = config.num_of_links - 2;
	link.type = 'R';
	link_seq_second = insertIntoWooF(data.lw_name, NULL, (void *)&link, cp_ht);
	if(link_seq_second == (unsigned long)-1) return VS_invalid();

	/* populate terminal node */
	node.dw_seq_no = data_seq;
	node.lw_seq_no = link_seq_first;
	stack = Stack_init(sizeof(NODE));
	status = populate_terminal_node(di, root_ap, &terminal_node, stack);
	if(status == 0) return VS_invalid();

	/* connect terminal node and new node */
	add_node_return = add_node(working_vs, terminal_node, node, stack, 'N', root_ap.node, &head_changed);
	if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();

	/* insert new AP */
	if(add_node_return.dw_seq_no == 0){ // new node added to empty tree: update root AP
		memcpy((void *)&root_ap.node, (void *)&node, sizeof(NODE));
	}else if(head_changed){ // root changed due to copy
		memcpy((void *)&root_ap.node, (void *)&add_node_return, sizeof(NODE));
	}
	memcpy((void *)&root_ap.vs, (void *)&working_vs, sizeof(VS));
	idx = insertIntoWooF(AP_WOOF_NAME, NULL, (void *)&root_ap, cp_ht);
	if(idx == (unsigned long)-1) return VS_invalid();

	return working_vs;

}

VS BST_delete(DI di, VS vs){

	AP root_ap;
	bool head_changed;
	DATA parent_data;
	DATA target_data;
	int status;
	LINK left;
	LINK right;
	LINK pred_left;
	LINK pred_right;
	NODE parent;
	NODE pred;
	NODE pred_parent;
	NODE target;
	NODE add_node_return;
	Stack *stack;
	Stack *pred_stack;
	Stack *auxiliary_stack;
	StackNode *popped;
	unsigned long idx;
	VS working_vs;
	VS current_vs;

	/* initialize */
	status = load_config(false);
	if(status == 0) return VS_invalid();
	memset((void *)&parent, 0, sizeof(NODE));
	memset((void *)&pred_parent, 0, sizeof(NODE));
	head_changed = false;
	stack = NULL;
	pred_stack = NULL;
	auxiliary_stack = NULL;

	status = populate_latest_AP((void *)&root_ap);
	if(status == 0) return VS_invalid();

	memcpy((void *)&current_vs, (void *)&root_ap.vs, sizeof(VS));
	status = populate_working_VS((void *)&working_vs, current_vs, vs);

	cp_ht = NULL;

	/* search for the node to be deleted */
	stack = Stack_init(sizeof(NODE));
	status = search(di, current_vs, &target, stack);
	if(status == 0) return VS_invalid();
	if(target.dw_seq_no == 0) return working_vs; // not found 

	/* pop target, as it is already populated can be destroyed - we want to get the parent */
	popped = Stack_pop(stack);
	if(popped) StackNode_destroy(popped);
	/* pop parent, then push it back again as it is the 'terminal' node */
	popped = Stack_pop(stack);
	if(popped){
		memcpy((void *)&parent, (void *)popped->element, sizeof(NODE));
		Stack_push(stack, (void *)&parent);
		StackNode_destroy(popped);
	}

	/* populate current children of target */
	status = populate_current_links(current_vs, target, &left, &right);
	if(status == 0) return VS_invalid();

	/* populate target data */
	status = readFromWooF(DATA_WOOF_NAME, (void *)&target_data, target.dw_seq_no);
	if(status < 0) return VS_invalid();

	if(left.node.dw_seq_no == 0 && right.node.dw_seq_no == 0){ // none of the children are present
		if(parent.dw_seq_no == 0){ // target is the root
			head_changed = true;
			memset((void *)&add_node_return, 0, sizeof(NODE));
		}else{
			status = readFromWooF(DATA_WOOF_NAME, (void *)&parent_data, parent.dw_seq_no);
			if(status < 0) return VS_invalid();
			/* add null node to parent, in this case both left and right are null */
			if(DI_cmp(target_data.di, parent_data.di) < 0){
				add_node_return = add_node(working_vs, parent, left.node, stack, 'L', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}else{
				add_node_return = add_node(working_vs, parent, left.node, stack, 'R', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}
		}
	}else if(left.node.dw_seq_no != 0 && right.node.dw_seq_no == 0){ // only the left child is present
		if(parent.dw_seq_no == 0){ // target is the root
			head_changed = true;
			memcpy((void *)&add_node_return, (void *)&left.node, sizeof(NODE));
		}else{
			status = readFromWooF(DATA_WOOF_NAME, (void *)&parent_data, parent.dw_seq_no);
			if(status < 0) return VS_invalid();
			if(DI_cmp(target_data.di, parent_data.di) < 0){ // target is a left child
				add_node_return = add_node(working_vs, parent, left.node, stack, 'L', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}else{ // target is a right child
				add_node_return = add_node(working_vs, parent, left.node, stack, 'R', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}
		}
	}else if(left.node.dw_seq_no == 0 && right.node.dw_seq_no != 0){ // only the right child is present
		if(parent.dw_seq_no == 0){ // target is the root
			head_changed = true;
			memcpy((void *)&add_node_return, (void *)&right.node, sizeof(NODE));
		}else{
			status = readFromWooF(DATA_WOOF_NAME, (void *)&parent_data, parent.dw_seq_no);
			if(status < 0) return VS_invalid();
			if(DI_cmp(target_data.di, parent_data.di) < 0){ // target is a left child
				add_node_return = add_node(working_vs, parent, right.node, stack, 'L', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}else{ // target is a right child
				add_node_return = add_node(working_vs, parent, right.node, stack, 'R', root_ap.node, &head_changed);
				if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			}
		}
	}else{ // both children are present
		pred_stack = Stack_init(sizeof(NODE));
		status = populate_predecessor(working_vs, target, &pred, pred_stack);

		if(pred.dw_seq_no == left.node.dw_seq_no){ // pred is immediate left child
			// step 1: add (target->right) to (predecessor)
			auxiliary_stack = Stack_init(sizeof(NODE));
			Stack_push(auxiliary_stack, (void *)&pred);
			add_node_return = add_node(working_vs, pred, right.node, auxiliary_stack, 'R', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			if(add_node_return.dw_seq_no > 0) memcpy((void *)&pred, (void *)&add_node_return, sizeof(NODE)); // pred might have been copied
			// step 2: add (predecessor) to (parent)
			if(parent.dw_seq_no != 0){
				add_node_return = add_node(working_vs, parent, pred, stack, 'N', root_ap.node, &head_changed);
			}else{
				head_changed = true;
			}
		}else{ // pred is not the immediate left child
			// pred will not have right child, might have left child
			status = populate_current_links(working_vs, pred, &pred_left, &pred_right);
			/* pop pred, as it is already populated can be destroyed - we want to get the parent */
			popped = Stack_pop(pred_stack);
			if(popped) StackNode_destroy(popped);
			/* pop parent of pred, then push it back again as it is the 'terminal' node */
			popped = Stack_pop(pred_stack);
			if(popped){
				memcpy((void *)&pred_parent, (void *)popped->element, sizeof(NODE));
				Stack_push(pred_stack, (void *)&pred_parent);
				StackNode_destroy(popped);
			}

			// step 1: add (pred->left) to (pred->parent) -- notice both (pred->parent) and (target->left) might change
			add_node_return = add_node(working_vs, pred_parent, pred_left.node, pred_stack, 'R', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			idx = get_latest_lw_seq_no(pred_parent);
			if(idx == (unsigned long)-1) return VS_invalid();
			else pred_parent.lw_seq_no = idx;
			//left.node.lw_seq_no = add_node_return.lw_seq_no;
			idx = get_latest_lw_seq_no(left.node);
			if(idx == (unsigned long)-1) return VS_invalid();
			else left.node.lw_seq_no = idx;

			// step 2a: add (target->left) to (pred)
			auxiliary_stack = Stack_init(sizeof(NODE));
			Stack_push(auxiliary_stack, (void *)&pred);
			add_node_return = add_node(working_vs, pred, left.node, auxiliary_stack, 'L', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			pred.lw_seq_no = add_node_return.lw_seq_no; // pred lw_seq_no might change
			Stack_destroy(auxiliary_stack);

			// step 2b: add (target->right) to (pred)
			auxiliary_stack = Stack_init(sizeof(NODE));
			Stack_push(auxiliary_stack, (void *)&pred);
			add_node_return = add_node(working_vs, pred, right.node, auxiliary_stack, 'R', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
			pred.lw_seq_no = add_node_return.lw_seq_no; // pred lw_seq_no might change
			Stack_destroy(auxiliary_stack);

			// step 3: add (pred) to (target->parent)
			if(parent.dw_seq_no != 0){
				add_node_return = add_node(working_vs, parent, pred, stack, 'N', root_ap.node, &head_changed);
			}else{
				head_changed = true;
			}
		}
	}

	/* insert new ap */
	if(head_changed){ // root changed due to copy
		memcpy((void *)&root_ap.node, (void *)&add_node_return, sizeof(NODE));
	}
	memcpy((void *)&root_ap.vs, (void *)&working_vs, sizeof(VS));
	idx = insertIntoWooF(AP_WOOF_NAME, NULL, (void *)&root_ap, cp_ht);
	if(idx == (unsigned long)-1) return VS_invalid();

	return working_vs;

}

unsigned long get_latest_lw_seq_no(NODE node){
	
	DATA data;
	int status;
	LINK link;
	unsigned long i;

	status = load_config(false);
	if(status == 0) return (unsigned long)-1;

	status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
	if(status < 0) return (unsigned long)-1;

	i = getLatestSeqNo(data.lw_name);
	if(i == (unsigned long)-1) return (unsigned long)-1;

	while(i){
		status = readFromWooF(data.lw_name, (void *)&link, i);
		if(status < 0) return (unsigned long)-1;
		if(link.num_of_remaining_links == (config.num_of_links - 1)) return i;
		--i;
	}

	return i;

}

int populate_predecessor(VS vs, NODE target, NODE *pred, Stack *pred_stack){

	int status;
	LINK left;
	LINK right;
	NODE iterator;

	memset((void *)pred, 0, sizeof(NODE));

	status = populate_current_links(vs, target, &left, &right);
	if(status == 0) return 0;

	/* left child will always be present, as this function is called only when both children are present */
	memcpy((void *)&iterator, (void *)&left.node, sizeof(NODE));

	while(1){
		Stack_push(pred_stack, (void *)&iterator);
		status = populate_current_links(vs, iterator, &left, &right);
		if(status == 0) return 0;
		if(right.node.dw_seq_no == 0) break;
		memcpy((void *)&iterator, (void *)&right.node, sizeof(NODE));
	}

	memcpy((void *)pred, (void *)&iterator, sizeof(NODE));

	return 1;

}

int accumulate_preorder(VS vs, NODE node, Accumulator *acc){

	DATA data;
	int status;
	LINK left;
	LINK right;

	if(node.dw_seq_no == 0) return 1;
	
	status = populate_current_links(vs, node, &left, &right);
	if(status == 0) return 0;

	status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
	if(status < 0) return 0;
	Accumulator_insert(acc, (void *)&data.di);

	accumulate_preorder(vs, left.node, acc);
	accumulate_preorder(vs, right.node, acc);

	return 1;

}

Accumulator *preorder_retrieve(VS vs){
	
	AP head;
	int status;
	Accumulator *acc;

	status = load_config(false);
	if(status == 0) return NULL;
	memset((void *)&head, 0, sizeof(AP));

	status = (config.num_of_extra_links == 0) ? populate_latest_AP(&head) : populate_AP(vs, &head);
	if(status == 0) return NULL;

	acc = Accumulator_init(sizeof(DI));
	status = accumulate_preorder(vs, head.node, acc);
	if(status == 0) return NULL;

	return acc;

}

int BST_preorder(VS vs){
	
	Accumulator *acc;
	AccumulatorNode *acc_node;
	char *str;

	acc = preorder_retrieve(vs);

	acc_node = acc->head;
	fprintf(stdout, "%s: ", VS_str(vs));
	while(acc_node != NULL){
		str = DI_str(*(DI *)acc_node->element);
		fprintf(stdout, "%s", str);
		free(str);
		acc_node = acc_node->next;
	}
	fprintf(stdout, "\n");

	Accumulator_destroy(acc);

	return 1;

}

int populate_AP(VS vs, AP *ap){

	int status;
	unsigned long idx;
	unsigned long latest_seq;

	idx = 0;
	status = load_config(false);
	if(status == 0) return 0;
	memset((void *)ap, 0, sizeof(AP));

	latest_seq = getLatestSeqNo(AP_WOOF_NAME);
	if(latest_seq == (unsigned long)-1) return 0;
	idx = vs.counter;

	while(idx <= latest_seq){
		memset((void *)ap, 0, sizeof(AP));
		status = readFromWooF(AP_WOOF_NAME, (void *)ap, idx);
		if(status < 0) return 0;
		if(VS_cmp(ap->vs, vs) == 0) return 1;
		++idx;
	}

	return 0;

}

int populate_latest_AP(AP *ap){

	unsigned long latest_seq_no;
	int read_status;

	memset((void *)ap, 0, sizeof(AP));
	ap->vs.nodeID = config.nodeID;

	latest_seq_no = getLatestSeqNo(AP_WOOF_NAME);
	if(latest_seq_no == (unsigned long)-1){ // op failed
		ap->vs.nodeID = INVALID_NODE_ID;
		return 0;
	}
	if(latest_seq_no == 0){	// empty ap
		return 1;
	}

	read_status = readFromWooF(AP_WOOF_NAME, (void *)ap, latest_seq_no);
	if(read_status < 0){ // op failed
		ap->vs.nodeID = INVALID_NODE_ID;
		return 0;
	}

	return 1;

}

int populate_working_VS(VS *vs, VS latest_vs, VS assigned_vs){

	unsigned long counter;
	VS invalid;

	invalid = VS_invalid();
	if(VS_cmp(assigned_vs, invalid) != 0){
		memcpy((void *)vs, (void *)&assigned_vs, sizeof(VS));
		return 1;
	}

	memset((void *)vs, 0, sizeof(VS));
	memset((void *)&counter, 0, sizeof(unsigned long));
	vs->nodeID = config.nodeID;

	vs->counter = latest_vs.counter + 1;

	return 1;

}

int load_config(bool reload){

	int status;

	if(!reload && config.nodeID != 0) return 1;

	status = getLastEntry(CONFIG_WOOF_NAME, (void *)&config);
	if(status == -1) return 0;
	return 1;

}

unsigned long get_VS_order(VS vs, bool *failed){

	int status;

	*failed = false;

	status = load_config(false);
	if(status == 0){
		*failed = true;
		return (unsigned long)-1;
	}

	return vs.counter;

}

int populate_current_links(VS vs, NODE node, LINK *left, LINK *right){
	
	bool left_seen;
	bool right_seen;
	DATA data;
	int status;
	LINK iterator;
	unsigned long candidate_vs_order;
	unsigned long i;
	unsigned long latest_seq;
	unsigned long vs_order;
	bool failed;

	status = load_config(false);
	if(status == 0) return 0;

	memset((void *)left, 0, sizeof(LINK));
	left->vs.nodeID = INVALID_NODE_ID;
	left->dw_vs.nodeID = INVALID_NODE_ID;
	left->num_of_remaining_links = config.num_of_links;
	left->type = 'L';
	memset((void *)right, 0, sizeof(LINK));
	right->vs.nodeID = INVALID_NODE_ID;
	right->dw_vs.nodeID = INVALID_NODE_ID;
	right->num_of_remaining_links = config.num_of_links;
	right->type = 'R';

	if(node.dw_seq_no == 0) return 1; // invalid node

	status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
	if(status < 0) return 0;
	latest_seq = getLatestSeqNo(data.lw_name);
	if(latest_seq == (unsigned long)-1) return 0;

	vs_order = get_VS_order(vs, &failed);
	if(failed) return 0;

	if(config.num_of_extra_links > 0){
		i = 0;
		while(1){
			if((node.lw_seq_no + i) > latest_seq) break; // end of woof
			
			status = readFromWooF(data.lw_name, (void *)&iterator, node.lw_seq_no + i);
			if(status < 0) return 0;

			if((iterator.num_of_remaining_links == (config.num_of_links - 1)) && (i != 0)) break; // end of this copy of node

			candidate_vs_order = get_VS_order(iterator.vs, &failed);
			if(failed) return 0;
			if(candidate_vs_order <= vs_order){
				if(iterator.type == 'L') memcpy((void *)left, (void *)&iterator, sizeof(LINK));
				else memcpy((void *)right, (void *)&iterator, sizeof(LINK));
			}

			i += 1;
		}
	}else{
		i = latest_seq;
		left_seen = false;
		right_seen = false;
		while(!left_seen || !right_seen){
			status = readFromWooF(data.lw_name, (void *)&iterator, i);
			if(status < 0) return 0;
			if(!left_seen && iterator.type == 'L'){
				left_seen = true;
				memcpy((void *)left, (void *)&iterator, sizeof(LINK));
			}else if(!right_seen && iterator.type == 'R'){
				right_seen = true;
				memcpy((void *)right, (void *)&iterator, sizeof(LINK));
			}
			--i;
		}
	}

	return 1;

}

int populate_terminal_node(DI di, AP root_ap, NODE *terminal_node, Stack *stack){
	
	NODE node;
	int status;
	LINK left;
	LINK right;
	DATA data;

	memset((void *)terminal_node, 0, sizeof(NODE));

	if(root_ap.node.dw_seq_no == 0) return 1; // empty data structure

	memcpy((void *)&node, (void *)&root_ap.node, sizeof(NODE));

	while(1){
		Stack_push(stack, (void *)&node);
		
		status = populate_current_links(root_ap.vs, node, &left, &right);
		if(status == 0) return 0;
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
		if(status < 0) return 0;

		if(DI_cmp(di, data.di) < 0){
			if(left.node.dw_seq_no == 0) break;
			memcpy((void *)&node, (void *)&left.node, sizeof(NODE));
		}else{
			if(right.node.dw_seq_no == 0) break;
			memcpy((void *)&node, (void *)&right.node, sizeof(NODE));
		}
	}

	memcpy((void *)terminal_node, (void *)&node, sizeof(NODE));

	return 1;

}

NODE add_node(VS working_vs, NODE terminal_node, NODE new_node, Stack *stack, char type, NODE current_head, bool *head_changed){

	bool new_copy_flag;
	DATA terminal_data;
	DATA new_data;
	int status;
	LINK left;
	LINK right;
	LINK terminal_last_link;
	LINK terminal_link;
	NODE retval;
	StackNode *popped_sn;
	unsigned long first_link_seq;
	unsigned long new_link_seq;

	new_copy_flag = false;
	*head_changed = false;
	memset((void *)&retval, 0, sizeof(NODE));
	status = load_config(false);
	if(status == 0) return NODE_failed_op();

	if(terminal_node.dw_seq_no == 0){ // NULL node
		*head_changed = true;
		return retval;
	}

	if(config.num_of_extra_links == 0){ // ephemeral
		/* populate terminal/new data and terminal last link */
		status = readFromWooF(DATA_WOOF_NAME, (void *)&terminal_data, terminal_node.dw_seq_no);
		if(status < 0) return NODE_failed_op();
		status = getLastEntry(terminal_data.lw_name, (void *)&terminal_last_link);
		if(status == (unsigned long)-1) return NODE_failed_op();
		/* find link type */
		if(new_node.dw_seq_no != 0){
			status = readFromWooF(DATA_WOOF_NAME, (void *)&new_data, new_node.dw_seq_no);
			if(status < 0) return NODE_failed_op();
			type = (DI_cmp(new_data.di, terminal_data.di) < 0) ? 'L' : 'R';
		}
		/* populate link to be inserted */
		memcpy((void *)&terminal_last_link.vs, (void *)&working_vs, sizeof(VS));
		memcpy((void *)&terminal_last_link.node, (void *)&new_node, sizeof(NODE));
		terminal_last_link.type = type;
		/* insert link */
		new_link_seq = insertIntoWooF(terminal_data.lw_name, NULL, (void *)&terminal_last_link, cp_ht);
		if(new_link_seq == (unsigned long)-1) return NODE_failed_op();
		retval.dw_seq_no = terminal_node.dw_seq_no;
		retval.lw_seq_no = new_link_seq;
		return retval;
	}

	/* PERSISTENT */
	while(1){
		/* pop to get current terminal node */
		popped_sn = Stack_pop(stack);
		if(popped_sn == NULL) break;
		memcpy((void *)&terminal_node, (void *)popped_sn->element, sizeof(NODE));
		StackNode_destroy(popped_sn);

		/* populate new/terminal data and terminal last link */
		status = readFromWooF(DATA_WOOF_NAME, (void *)&terminal_data, terminal_node.dw_seq_no);
		if(status < 0) return NODE_failed_op();
		status = getLastEntry(terminal_data.lw_name, (void *)&terminal_last_link);
		if(status == (unsigned long)-1) return NODE_failed_op();

		/* populate current left right */
		populate_current_links(working_vs, terminal_node, &left, &right);

		/* populate link type (L or R) if unknown */
		if(new_node.dw_seq_no != 0){
			status = readFromWooF(DATA_WOOF_NAME, (void *)&new_data, new_node.dw_seq_no);
			if(status < 0) return NODE_failed_op();
			type = (DI_cmp(new_data.di, terminal_data.di) < 0) ? 'L' : 'R';
		}

		/* populate partial link */
		memcpy((void *)&terminal_link.vs, (void *)&working_vs, sizeof(VS));
		memcpy((void *)&terminal_link.dw_vs, (void *)&terminal_last_link.dw_vs, sizeof(VS));
		memcpy((void *)&terminal_link.node, (void *)&new_node, sizeof(NODE));
		terminal_link.num_of_remaining_links = 0;
		terminal_link.type = type;

		if(terminal_last_link.num_of_remaining_links > 0){ // no need to copy
			new_copy_flag = false;
			terminal_link.num_of_remaining_links = terminal_last_link.num_of_remaining_links - 1;
			first_link_seq = terminal_node.lw_seq_no;
		}else{
			new_copy_flag = true;
			if(type == 'L'){ // copy right
				right.num_of_remaining_links = config.num_of_links - 1;
				first_link_seq = insertIntoWooF(terminal_data.lw_name, NULL, (void *)&right, cp_ht);
				if(first_link_seq == (unsigned long)-1) return NODE_failed_op();
			}else{ // copy left
				left.num_of_remaining_links = config.num_of_links - 1;
				first_link_seq = insertIntoWooF(terminal_data.lw_name, NULL, (void *)&left, cp_ht);
				if(first_link_seq == (unsigned long)-1) return NODE_failed_op();
			}
			terminal_link.num_of_remaining_links = config.num_of_links - 2;
		}
		
		/* insert new link */
		new_link_seq = insertIntoWooF(terminal_data.lw_name, NULL, (void *)&terminal_link, cp_ht);
		if(new_link_seq == (unsigned long)-1) return NODE_failed_op();

		/* no need to iterate if we worked on the head or no copy created */
		if(! new_copy_flag) break;
		if(NODE_is_equal(terminal_node, current_head)){
			*head_changed = true;
			break;
		}

		new_node.dw_seq_no = terminal_node.dw_seq_no;
		new_node.lw_seq_no = (! new_copy_flag) ? new_link_seq : first_link_seq;
	}

	retval.dw_seq_no = terminal_node.dw_seq_no;
	retval.lw_seq_no = first_link_seq;
	return retval;

}

void BST_traverse_max(int *num){

	int status;
	AP root_ap;
	DATA data;
	LINK link;
	NODE node;
	unsigned long seq_no;

	*num = 0;

	status = populate_latest_AP((void *)&root_ap);
	if(status == 0) return;

	node = root_ap.node;

	while(node.dw_seq_no != 0){
		*num += 1;
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
		if(status < 0) return;
		
		seq_no = getLatestSeqNo(data.lw_name);
		while(seq_no != 0){
			status = readFromWooF(data.lw_name, (void *)&link, seq_no);
			if(link.type == 'R') break;
			seq_no -= 1;
		}

		node = link.node;
	}

}
