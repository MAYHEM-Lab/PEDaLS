#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Accumulator.h"
#include "LinkedList.h"
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

int LL_init(
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
	config.num_of_links = 1 + num_of_extra_links;			// for linked list default 1
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

/**
 * returns 1 on success, 0 on failure
 **/
int populate_current_link(VS vs, NODE node, LINK *link){
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

	memset((void *)link, 0, sizeof(LINK));
	link->vs.nodeID = INVALID_NODE_ID;
	link->dw_vs.nodeID = INVALID_NODE_ID;
	link->num_of_remaining_links = config.num_of_links;
	link->type = 'N';

	if(node.dw_seq_no == 0) return 1; // invalid node

	status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
	if(status < 0) return 0;
	latest_seq = getLatestSeqNo(data.lw_name);
	if(latest_seq == (unsigned long)-1) return 0;

	vs_order = get_VS_order(vs, &failed);
	if(failed) return 0;

	i = 0;
	while(1){
		if((node.lw_seq_no + i) > latest_seq) break; // end of woof
		
		status = readFromWooF(data.lw_name, (void *)&iterator, node.lw_seq_no + i);
		if(status < 0) return 0;

		if((iterator.num_of_remaining_links == (config.num_of_links - 1)) && (i != 0)) break; // end of this copy of node

		candidate_vs_order = get_VS_order(iterator.vs, &failed);
		if(failed) return 0;
		if(candidate_vs_order > vs_order) return 1;
		if(candidate_vs_order <= vs_order){
			memcpy((void *)link, (void *)&iterator, sizeof(LINK));
		}

		i += 1;
	}

	return 1;
}

int populate_terminal_node(DI di, AP root_ap, NODE *terminal_node, Stack *stack){
	
	NODE node;
	int status;
	LINK next;
	DATA data;

	memset((void *)terminal_node, 0, sizeof(NODE));

	if(root_ap.node.dw_seq_no == 0) return 1; // empty data structure

	memcpy((void *)&node, (void *)&root_ap.node, sizeof(NODE));

	while(1){
		Stack_push(stack, (void *)&node);
		
		status = populate_current_link(root_ap.vs, node, &next);
		if(status == 0) return 0;
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
		if(status < 0) return 0;

		if(next.node.dw_seq_no == 0) break;
		memcpy((void *)&node, (void *)&next.node, sizeof(NODE));
	}

	memcpy((void *)terminal_node, (void *)&node, sizeof(NODE));

	return 1;

}

int search(DI di, VS vs, NODE *node, Stack *stack){

	int status;
	AP head;
	DATA data;
	LINK next;
	NODE iterator;

	status = load_config(false);
	if(status == 0) return 0;
	memset((void *)node, 0, sizeof(NODE));

	status = (config.num_of_extra_links == 0) ? populate_latest_AP(&head) : populate_AP(vs, &head);
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
		status = populate_current_link(vs, iterator, &next);
		if(status == 0) return 0;
		memcpy((void *)&iterator, (void *)&next.node, sizeof(NODE));
	}

	return 1; // not found

}

unsigned long LL_search(DI di, VS vs){

	NODE node;
	Stack *stack;
	int status;

	stack = Stack_init(sizeof(NODE));
	status = search(di, vs, &node, stack);
	Stack_destroy(stack);

	if(status == 0) return (unsigned long)-1;

	return node.dw_seq_no;

}

NODE add_node(VS working_vs, NODE terminal_node, NODE new_node, Stack *stack, char type, NODE current_head, bool *head_changed){

	bool new_copy_flag;
	DATA terminal_data;
	int status;
	LINK next;
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

		/* populate current link */
		populate_current_link(working_vs, terminal_node, &next);

		type = 'N';

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
			terminal_link.num_of_remaining_links = config.num_of_links - 1;
		}
		
		/* insert new link */
		new_link_seq = insertIntoWooF(terminal_data.lw_name, NULL, (void *)&terminal_link, cp_ht);
		first_link_seq = new_link_seq;
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

VS LL_insert(DI di, VS vs){

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
	link.type = 'N';
	link_seq_first = insertIntoWooF(data.lw_name, NULL, (void *)&link, cp_ht);
	if(link_seq_first == (unsigned long)-1) return VS_invalid();

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

VS LL_delete(DI di, VS vs){

	AP root_ap;
	bool head_changed;
	DATA parent_data;
	DATA target_data;
	int status;
	LINK next;
	NODE parent;
	NODE target;
	NODE add_node_return;
	Stack *stack;
	StackNode *popped;
	unsigned long idx;
	VS working_vs;
	VS current_vs;

	/* initialize */
	status = load_config(false);
	if(status == 0) return VS_invalid();
	memset((void *)&parent, 0, sizeof(NODE));
	head_changed = false;
	stack = NULL;

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
	status = populate_current_link(current_vs, target, &next);
	if(status == 0) return VS_invalid();

	/* populate target data */
	status = readFromWooF(DATA_WOOF_NAME, (void *)&target_data, target.dw_seq_no);
	if(status < 0) return VS_invalid();

	if(next.node.dw_seq_no == 0){ // last element
		if(parent.dw_seq_no == 0){ // target is the root
			head_changed = true;
			memset((void *)&add_node_return, 0, sizeof(NODE));
		}else{
			status = readFromWooF(DATA_WOOF_NAME, (void *)&parent_data, parent.dw_seq_no);
			if(status < 0) return VS_invalid();
			/* add null node to parent */
			add_node_return = add_node(working_vs, parent, next.node, stack, 'N', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
		}
	}else{ // not last element
		if(parent.dw_seq_no == 0){ // target is the root
			head_changed = true;
			memcpy((void *)&add_node_return, (void *)&next.node, sizeof(NODE));
		}else{
			status = readFromWooF(DATA_WOOF_NAME, (void *)&parent_data, parent.dw_seq_no);
			if(status < 0) return VS_invalid();
			add_node_return = add_node(working_vs, parent, next.node, stack, 'N', root_ap.node, &head_changed);
			if(add_node_return.dw_seq_no == (unsigned long)-1) return VS_invalid();
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

Accumulator *accumulate_list(VS vs){
	
	AP head;
	DATA data;
	NODE node;
	int status;
	Accumulator *acc;
	LINK next;

	status = load_config(false);
	if(status == 0) return NULL;
	memset((void *)&head, 0, sizeof(AP));

	status = (config.num_of_extra_links == 0) ? populate_latest_AP(&head) : populate_AP(vs, &head);
	if(status == 0) return NULL;

	node = head.node;
	acc = Accumulator_init(sizeof(DI));
	while(1){
		if(node.dw_seq_no == 0) break;
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
		if(status < 0){
			Accumulator_destroy(acc);
			return NULL;
		}
		Accumulator_insert(acc, (void *)&data.di);
		status = populate_current_link(vs, node, &next);
		node = next.node;
		if(status == 0){
			Accumulator_destroy(acc);
			return NULL;
		}
	}

	return acc;

}

int LL_print(VS vs){
	
	Accumulator *acc;
	AccumulatorNode *acc_node;
	char *str;

	fprintf(stdout, "%lu%c: ", vs.counter, vs.nodeID);
	fflush(stdout);

	acc = accumulate_list(vs);

	acc_node = (acc == NULL) ? NULL : acc->head;
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

void debug_LINK(char *name){

    unsigned long i;
    LINK link;

    fprintf(stdout, "********************\n");
    fprintf(stdout, "DEBUGGING LINK %s\n", name);
    for(i = 1; i <= getLatestSeqNo(name); ++i){
        readFromWooF(name, (void *)&link, i);
        fprintf(stdout, "%lu: %lu%c %lu %lu %d %c\n", 
			i, link.vs.counter, link.vs.nodeID, link.node.dw_seq_no, link.node.lw_seq_no, link.num_of_remaining_links, link.type);
    }
    fflush(stdout);

}

void debug_LINKs(){

    unsigned long i;
    DATA data;

    for(i = 1; i <= getLatestSeqNo(DATA_WOOF_NAME); ++i){
        readFromWooF(DATA_WOOF_NAME, (void *)&data, i);
		debug_LINK(data.lw_name);
    }

}

void debug_DATA(){

    unsigned long i;
    DATA data;

    fprintf(stdout, "********************\n");
    fprintf(stdout, "DEBUGGING DATA\n");
    for(i = 1; i <= getLatestSeqNo(DATA_WOOF_NAME); ++i){
        readFromWooF(DATA_WOOF_NAME, (void *)&data, i);
        fprintf(stdout, "%lu: %lu%c %d %s\n", i, data.vs.counter, data.vs.nodeID, data.di.val, data.lw_name);
    }
    fflush(stdout);

}

void debug_AP(){

    unsigned long i;
    AP ap;

    fprintf(stdout, "********************\n");
    fprintf(stdout, "DEBUGGING AP\n");
    for(i = 1; i <= getLatestSeqNo(AP_WOOF_NAME); ++i){
        readFromWooF(AP_WOOF_NAME, (void *)&ap, i);
        fprintf(stdout, "%lu: %lu%c %lu %lu\n", i, ap.vs.counter, ap.vs.nodeID, ap.node.dw_seq_no, ap.node.lw_seq_no);
    }
    fflush(stdout);

}

void LL_debug(){
	debug_AP();
	debug_DATA();
	debug_LINKs();
}

void LL_traverse(int *num){

	int status;
	AP root_ap;
	DATA data;
	LINK link;
	NODE node;

	*num = 0;

	status = populate_latest_AP((void *)&root_ap);
	if(status == 0) return;

	node = root_ap.node;

	while(node.dw_seq_no != 0){
		*num += 1;
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, node.dw_seq_no);
		if(status < 0) return;
		
		status = getLastEntry(data.lw_name, (void *)&link);
		if(status != 1) return;

		node = link.node;
	}

}
