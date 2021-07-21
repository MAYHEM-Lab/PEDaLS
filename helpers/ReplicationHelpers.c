#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AccessPointer.h"
#include "Checkpoint.h"
#include "Data.h"
#include "DataItem.h"
#include "Knowledge.h"
#include "Link.h"
#include "Operation.h"
#include "Options.h"
#include "ReplicationHelpers.h"
#include "VersionStampIdxMap.h"

#define SYNC_REC_TEST 0
#define SYNC_REC_TEST_VAL 1000

unsigned long getAPPosFromVS(VS vs, bool *failed){

	char ap_woof_name[MAX_SIZE_WOOF_NAME];
	unsigned long latest_seq;
	unsigned long low;
	unsigned long high;
	unsigned long mid;
	VSIDX vsidx;
	int status;

	*failed = false;

	memset((void *)&vsidx, 0, sizeof(VSIDX));
	populateAPWooFName(vs.nodeID, ap_woof_name);
	latest_seq = getLatestSeqNo(ap_woof_name);
	if(latest_seq == (unsigned long)-1){
		*failed = true;
		return (unsigned long)-1;
	}
	if(latest_seq == 0){
		return 0;
	}

	low = 1;
	high = latest_seq;
	mid = low + (high - low) / 2;

	while(low <= high){
		status = readFromWooF(ap_woof_name, (void *)&vsidx, mid);
		if(status < 0){
			*failed = true;
			return (unsigned long)-1;
		}
		if(VS_cmp(vs, vsidx.vs) < 0){
			high = mid - 1;
		}else if(VS_cmp(vs, vsidx.vs) > 0){
			low = mid + 1;
		}else{
			return vsidx.idx;
		}
		mid = low + (high - low) / 2;
	}

	status = getLastEntry(ap_woof_name, (void *)&vsidx);
	if(status == -1){
		*failed = true;
		return (unsigned long)-1;
	}
	if(vs.counter >= vsidx.vs.counter) return ULONG_MAX;

	return 0;

}

unsigned long writeCP(CONFIG config, VS working_vs, HT *cp_ht){
	int i;
	int num_woofs;
	CP cp;
	SLOT *iterator;
	unsigned long idx;
	unsigned long first_idx;

	idx = 0;
	first_idx = 0;
	num_woofs = 0;

	for(i = 0; i < cp_ht->ht_size; ++i){
		iterator = cp_ht->table[i];
		while(iterator != NULL){
			num_woofs += 1;
			if(num_woofs == 1){//this is the first record
				memset((void *)&cp, 0, sizeof(cp));
				cp.vs = working_vs;
			}
			strcpy(cp.cpr[num_woofs - 1].WooFName, iterator->key);
			cp.cpr[num_woofs - 1].latest_seq_no = *(unsigned long*)iterator->value;
			if(num_woofs == CPR_PER_CP){//this is the last record
				//write it out
				idx = insertIntoWooF(CHECKPOINT_WOOF_NAME, NULL, (void *)&cp, NULL);
				if(idx == (unsigned long)-1) return idx;
				if(first_idx == 0) first_idx = idx;
				num_woofs = 0;
			}
			iterator = iterator->next;
		}
	}
	if(num_woofs > 0){
		//write it out
		idx = insertIntoWooF(CHECKPOINT_WOOF_NAME, NULL, (void *)&cp, NULL);
		if(idx == (unsigned long)-1) return idx;
		if(first_idx == 0) first_idx = idx;
	}
	return first_idx;
}

int opLastPhaseReplication(CONFIG config, VS working_vs, unsigned long ap_seq, int op_code, DI di, HT *cp_ht){

	char ap_woof_name[MAX_SIZE_WOOF_NAME];
	char cp_woof_name[MAX_SIZE_WOOF_NAME];
	char know_woof_name[MAX_SIZE_WOOF_NAME];
	KNOW know;
	OP op;
	unsigned long idx;
	VSIDX vsidx;

	memset((void *)&vsidx, 0, sizeof(VSIDX));

	/* update ap map */
	memcpy((void *)&vsidx.vs, (void *)&working_vs, sizeof(VS));
	vsidx.idx = ap_seq;
	sprintf(ap_woof_name, "%s%c", AP_WOOF_NAME, working_vs.nodeID);
	idx = insertIntoWooF(ap_woof_name, NULL, (void *)&vsidx, NULL);
	if(idx == (unsigned long)-1) return 0;

	/* write checkpoint */
	idx = writeCP(config, working_vs, cp_ht);
	if(idx == (unsigned long)-1) return 0;

	/* update cp map */
	vsidx.idx = idx;
	sprintf(cp_woof_name, "%s%c", CHECKPOINT_WOOF_NAME, working_vs.nodeID);
	idx = insertIntoWooF(cp_woof_name, NULL, (void *)&vsidx, NULL);
	if(idx == (unsigned long)-1) return 0;

	/* update op */
	memcpy((void *)&op.vs, (void *)&working_vs, sizeof(VS));
	memcpy((void *)&op.di, (void *)&di, sizeof(DI));
	op.op = op_code;
	idx = insertIntoWooF(OPERATION_WOOF_NAME, NULL, (void *)&op, NULL);
	if(idx == (unsigned long)-1) return 0;

	/* update knowledge */
	memcpy((void *)&know.vs, (void *)&working_vs, sizeof(VS));
	know.seq_no = idx;
	sprintf(know_woof_name, "%c", working_vs.nodeID);
	idx = insertIntoWooF(know_woof_name, NULL, (void *)&know, NULL);
	if(idx == (unsigned long)-1) return 0;

	return 1;

}

int populateIntendedPredecessor(char *woof_name, unsigned long earliest_conflict_seq_no, OP *intended_predecessor){

	int status;

	if(earliest_conflict_seq_no == 1){
		memset(intended_predecessor, 0, sizeof(OP));
		return 1;
	}

	status = readFromWooF(woof_name, (void *)intended_predecessor, earliest_conflict_seq_no - 1);
	if(status < 0) return -1;
	return 1;
}

int detectConflict(
	unsigned long *earliest_conflict_seq_no,
	unsigned long *earliest_conflict_seq_no_reader,
	TOP *top,
	CLIENT c,
	char local_nodeID,
	char remote_nodeID
){
	KNOW k_local;
	KNOW k_remote;
	char woof_name[MAX_SIZE_WOOF_NAME];
	char *temp;
	char earliest_conflict_nodeID;
	char str_nodeID[2];
	int i;
	int status;
	unsigned long idx;
	unsigned long latest_seq;
	OP intended_predecessor;
	OP intended_predecessor_reader;

	*earliest_conflict_seq_no = ULONG_MAX;
	*earliest_conflict_seq_no_reader = ULONG_MAX;
	earliest_conflict_nodeID = '?';
	memset(str_nodeID, 0, 2);
	memset(woof_name, 0, MAX_SIZE_WOOF_NAME);
	memset((void *)&intended_predecessor, 0, sizeof(OP));
	memset((void *)&intended_predecessor_reader, 0, sizeof(OP));

	for(i = 0; i < top->numNodes; ++i){
		memset(woof_name, 0, MAX_SIZE_WOOF_NAME);
		memset((void *)&k_remote, 0, sizeof(KNOW));
		str_nodeID[0] = top->nodeIDs[i];
		strcpy(woof_name, CLIENT_get_remote_woof_name(c, str_nodeID));
		status = getLastEntry(woof_name, (void *)&k_remote);
		if(status < 0){
			return -1;
		}

		memset(woof_name, 0, MAX_SIZE_WOOF_NAME);
		memset((void *)&k_local, 0, sizeof(KNOW));
		str_nodeID[0] = top->nodeIDs[i];
		strcpy(woof_name, CLIENT_get_local_woof_name(c, str_nodeID));
		status = getLastEntry(woof_name, (void *)&k_local);
		if(status < 0){
			return -1;
		}

		if(VS_cmp(k_local.vs, k_remote.vs) < 0){
			//TODO: why we need this block
			if(earliest_conflict_nodeID == '?'){
				earliest_conflict_nodeID = k_remote.vs.nodeID;
				*earliest_conflict_seq_no = k_remote.seq_no;
			}
			memset(woof_name, 0, MAX_SIZE_WOOF_NAME);
			str_nodeID[0] = top->nodeIDs[i];
			strcpy(woof_name, CLIENT_get_remote_woof_name(c, str_nodeID));
			latest_seq = getLatestSeqNo(woof_name);
			if(latest_seq == (unsigned long)-1) return -1;
			for(idx = latest_seq; idx > 0; --idx){
				memset((void *)&k_remote, 0, sizeof(KNOW));
				status = readFromWooF(woof_name, (void *)&k_remote, idx);
				if(status < 0) return -1;
				//if(k_remote.seq_no > k_local.seq_no){
				if(k_remote.vs.counter > k_local.vs.counter){
					if(k_remote.seq_no < *earliest_conflict_seq_no){
						*earliest_conflict_seq_no = k_remote.seq_no;
						earliest_conflict_nodeID = top->nodeIDs[i];
					}
				}else{
					break;
				}
			}
		}
	}

	if(earliest_conflict_nodeID != '?'){
		//earliest_conflict_seq_no found at this point
		//now determine earliest_conflict_seq_no_reader
		strcpy(woof_name, CLIENT_get_remote_woof_name(c, OPERATION_WOOF_NAME));
		status = populateIntendedPredecessor(woof_name, *earliest_conflict_seq_no, (void *)&intended_predecessor);
		if(status < 0) return -1;
		if(intended_predecessor.vs.counter == 0){//conflict from beginning
			*earliest_conflict_seq_no_reader = 1;
			return 1;
		}
		strcpy(woof_name, CLIENT_get_local_woof_name(c, OPERATION_WOOF_NAME));
		status = populateIntendedPredecessor(woof_name, *earliest_conflict_seq_no, (void *)&intended_predecessor_reader);
		if(status < 0) return -1;
		if(VS_cmp(intended_predecessor.vs, intended_predecessor_reader.vs) == 0){//conflict point same
			*earliest_conflict_seq_no_reader = *earliest_conflict_seq_no;
			return 1;
		}
		strcpy(woof_name, CLIENT_get_local_woof_name(c, OPERATION_WOOF_NAME));
		latest_seq = getLatestSeqNo(woof_name);
		if(latest_seq == (unsigned long)-1) return -1;
		for(idx = *earliest_conflict_seq_no + 1; idx <= latest_seq; ++idx){//conflict point different, walk forward to find conflict
			status = populateIntendedPredecessor(woof_name, idx, (void *)&intended_predecessor_reader);
			if(status < 0) return -1;
			if(VS_cmp(intended_predecessor.vs, intended_predecessor_reader.vs) == 0){
				*earliest_conflict_seq_no_reader = idx;
				return 1;
			}
		}

		return -1;
	}else{
		return 0;
	}
}

int pruneWooFs(char *woof_name, unsigned long element_size, unsigned long history_size, unsigned long prune_start_seq, char *base){
	
	int status;
	unsigned long insert_status;
	char temp_woof_name[MAX_SIZE_WOOF_NAME];
	void *element;
	unsigned long idx;
	char *filename;//for /cspot/
	char *temp_filename;//for /cspot/
	char moved_filename[MAX_SIZE_WOOF_NAME];
	char original_filename[MAX_SIZE_WOOF_NAME];
	char backup_folder[MAX_SIZE_WOOF_NAME];
	char intermediate_filename[MAX_SIZE_WOOF_NAME];
	DIR *dir;

	memset(backup_folder, 0, MAX_SIZE_WOOF_NAME);
	memset(intermediate_filename, 0, MAX_SIZE_WOOF_NAME);
	memset(moved_filename, 0, MAX_SIZE_WOOF_NAME);
	memset(original_filename, 0, MAX_SIZE_WOOF_NAME);
	memset(temp_woof_name, 0, MAX_SIZE_WOOF_NAME);
	filename = (char *)malloc((MAX_SIZE_WOOF_NAME + 8) * sizeof(char));
	memset(filename, 0, MAX_SIZE_WOOF_NAME + 8);
	temp_filename = (char *)malloc((MAX_SIZE_WOOF_NAME + 8) * sizeof(char));
	memset(temp_filename, 0, MAX_SIZE_WOOF_NAME + 8);
	strcpy(temp_woof_name, TEMP_WOOF_NAME);
	element = malloc(element_size);

	status = createWooF(temp_woof_name, element_size, history_size);
	if(status < 0) return 0;

	for(idx = 1; idx < prune_start_seq; ++idx){
		memset(element, 0, element_size);
		status = readFromWooF(woof_name, element, idx);
		if(status < 0) return 0;
		insert_status = insertIntoWooF(temp_woof_name, NULL, element, NULL);
		if(insert_status == (unsigned long)-1) return 0;
	}

	strcpy(backup_folder, base);
	strcat(backup_folder, "/");
	strcat(backup_folder, BACKUP_FOLDER_NAME);
	dir = opendir(backup_folder);
	if(! dir){//directory does not exist, so create
		status = mkdir(backup_folder, 0777);
		fflush(stdout);
	}else{
		closedir(dir);
	}
	strcpy(moved_filename, backup_folder);
	strcat(moved_filename, "/");
	strcat(moved_filename, woof_name);
	strcpy(original_filename, base);
	strcat(original_filename, "/");
	strcat(original_filename, woof_name);
	rename(original_filename, moved_filename);

	strcpy(temp_filename, base);
	strcat(temp_filename, "/");
	strcat(temp_filename, temp_woof_name);
	strcpy(intermediate_filename, backup_folder);
	strcat(intermediate_filename, "/");
	strcat(intermediate_filename, TEMP_WOOF_NAME);
	status = rename(temp_filename, intermediate_filename);
	status = rename(intermediate_filename, original_filename);

	free(temp_filename);
	free(filename);

	return 1;

}

unsigned long get_CP_seq_no_from_VS(VS vs, CONFIG config){

	char cp_woof_name[MAX_SIZE_WOOF_NAME];
	unsigned long latest_seq;
	unsigned long low;
	unsigned long high;
	unsigned long mid;
	VSIDX vsidx;
	int status;

	memset((void *)&vsidx, 0, sizeof(VSIDX));
	populateCPWooFName(vs.nodeID, cp_woof_name);
	latest_seq = getLatestSeqNo(cp_woof_name);
	if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
	if(latest_seq == 0){
		return 0;
	}

	low = 1;
	high = latest_seq;
	mid = low + (high - low) / 2;

	while(low <= high){
		status = readFromWooF(cp_woof_name, (void *)&vsidx, mid);
		if(status < 0) return (unsigned long)-1;
		if(VS_cmp(vs, vsidx.vs) < 0){
			high = mid - 1;
		}else if(VS_cmp(vs, vsidx.vs) > 0){
			low = mid + 1;
		}else{
			return vsidx.idx;
		}
		mid = low + (high - low) / 2;
	}

	return 0;

}

void populateAPWooFName(char nodeID, char *ap_woof_name){

	char suffix[2];

	memset((void *)ap_woof_name, 0, MAX_SIZE_WOOF_NAME);
	memset((void *)suffix, 0, 2);

	suffix[0] = nodeID;
	strcpy(ap_woof_name, AP_WOOF_NAME);
	strcat(ap_woof_name, suffix);

}

void populateCPWooFName(char nodeID, char *cp_woof_name){

	char suffix[2];

	memset((void *)cp_woof_name, 0, MAX_SIZE_WOOF_NAME);
	memset((void *)suffix, 0, 2);

	suffix[0] = nodeID;
	strcpy(cp_woof_name, CHECKPOINT_WOOF_NAME);
	strcat(cp_woof_name, suffix);

}

OP getIntendedPredecessorFromList(OP *op_list, unsigned long idx){
	
	OP op;

	memset((void *)&op, 0, sizeof(OP));
	if(idx == 0) return op;
	memcpy((void *)&op, (void *)&op_list[idx - 1], sizeof(OP));

	return op;

}

// returns number of distinct entries in merged list
unsigned long mergeOPList(
	OP *reader_op_list, 
	unsigned long reader_op_list_length, 
	OP *source_op_list, 
	unsigned long source_op_list_length,
	unsigned long *first_remote_insertion
){

	unsigned long reader_idx;
	unsigned long source_idx;
	unsigned long idx;
	unsigned long current_num;
	OP reader_pred;
	OP source_pred;
	
	*first_remote_insertion = (unsigned long)-1;

	//handle reader empty case
	if(reader_op_list_length == 0){
		*first_remote_insertion = 0;
		for(source_idx = 0; source_idx < source_op_list_length; ++source_idx){
			memcpy((void *)&reader_op_list[source_idx], (void *)&source_op_list[source_idx], sizeof(OP));
		}
		return source_op_list_length;
	}

	current_num = reader_op_list_length;

	for(source_idx = 0, reader_idx = 0; source_idx < source_op_list_length; ++source_idx){
		source_pred = getIntendedPredecessorFromList(source_op_list, source_idx);
		reader_pred = getIntendedPredecessorFromList(reader_op_list, reader_idx);
		while(VS_cmp(source_pred.vs, reader_pred.vs) != 0){
			reader_idx += 1;
			reader_pred = getIntendedPredecessorFromList(reader_op_list, reader_idx);
		}
		while((reader_idx < current_num) && (VS_cmp(reader_op_list[reader_idx].vs, source_op_list[source_idx].vs) > 0)){
			reader_idx += 1;
		}
		if(reader_idx == current_num){//reached end of reader list to find place to insert source element
			memcpy((void *)&reader_op_list[reader_idx], (void *)&source_op_list[source_idx], sizeof(OP));
			if(*first_remote_insertion == (unsigned long)-1) *first_remote_insertion = reader_idx;
			current_num += 1;
			reader_idx += 1;
		}else if(VS_cmp(reader_op_list[reader_idx].vs, source_op_list[source_idx].vs) == 0){//already present, no need to add
			reader_idx += 1;
		}else if(VS_cmp(reader_op_list[reader_idx].vs, source_op_list[source_idx].vs) < 0){
			for(idx = current_num; idx > reader_idx; --idx){
				reader_op_list[idx] = reader_op_list[idx - 1];
				memcpy((void *)&reader_op_list[idx], (void *)&reader_op_list[idx - 1], sizeof(OP));
			}
			memcpy((void *)&reader_op_list[reader_idx], (void *)&source_op_list[source_idx], sizeof(OP));
			if(*first_remote_insertion == (unsigned long)-1) *first_remote_insertion = reader_idx;
			current_num += 1;
			reader_idx += 1;
		}
	}

	return current_num;

}

int resolveConflict(
	unsigned long earliest_conflict_seq_no,
	unsigned long earliest_conflict_seq_no_reader,
	TOP *top,
	CLIENT c,
	char local_nodeID,
	char remote_nodeID,
	VS (*insertFunction) (DI, VS),
	VS (*deleteFunction) (DI, VS),
	char *base
){

	OP *source_op_list; //earliest_conflict_seq_no to tail
	OP *reader_op_list; //earliest_conflict_seq_no_reader to tail
	unsigned long source_op_list_length; //partial list length
	unsigned long reader_op_list_length; //partial list length
	unsigned long merged_list_length;
	unsigned long idx;
	unsigned long latest_seq;
	unsigned long idx_cp_map;
	unsigned long latest_seq_cp_map;
	unsigned long idx_ap_map;
	unsigned long latest_seq_ap_map;
	unsigned long first_remote_insertion;
	unsigned long cp_prune_seq_no;
	char woof_name[MAX_SIZE_WOOF_NAME];
	char *temp;
	int status;
	int i;
	OP op;
	OP op_temp;
	CP cp;
	bool cp_found;
	HT *prune_ht;
	SLOT *slot;
	unsigned long element_size;
	unsigned long history_size;
	CONFIG config;
	char cp_woof_name[MAX_SIZE_WOOF_NAME];
	char ap_woof_name[MAX_SIZE_WOOF_NAME];
	char file_to_be_deleted[MAX_SIZE_WOOF_NAME];
	char delete_folder[MAX_SIZE_WOOF_NAME];
	char backup_folder[MAX_SIZE_WOOF_NAME];
	VSIDX vsidx;
	DIR *dir;
	struct dirent *de;
	bool failed;

	failed = false;
	memset(file_to_be_deleted, 0, MAX_SIZE_WOOF_NAME);
	memset(delete_folder, 0, MAX_SIZE_WOOF_NAME);
	memset(backup_folder, 0, MAX_SIZE_WOOF_NAME);
	memset((void *)woof_name, 0, MAX_SIZE_WOOF_NAME);
	memset((void *)&latest_seq, 0, sizeof(unsigned long));
	memset((void *)&first_remote_insertion, 0, sizeof(unsigned long));
	memset((void *)&config, 0, sizeof(CONFIG));

	temp = CLIENT_get_remote_woof_name(c, OPERATION_WOOF_NAME);
	memcpy(woof_name, temp, MAX_SIZE_WOOF_NAME);
	free(temp);
	latest_seq = getLatestSeqNo(woof_name);
	if(latest_seq == (unsigned long)-1) return 0;
	source_op_list_length = latest_seq - earliest_conflict_seq_no + 1;
	source_op_list = (OP *)malloc(source_op_list_length * sizeof(OP));
	for(idx = earliest_conflict_seq_no, i = 0; idx <= latest_seq; ++idx, ++i){
		status = readFromWooF(woof_name, (void *)&source_op_list[i], idx);
		if(status < 0) return 0;
	}

	temp = CLIENT_get_local_woof_name(c, OPERATION_WOOF_NAME);
	memcpy(woof_name, temp, MAX_SIZE_WOOF_NAME);
	free(temp);
	latest_seq = getLatestSeqNo(woof_name);
	if(latest_seq == (unsigned long)-1) return 0;
	reader_op_list_length = latest_seq - earliest_conflict_seq_no_reader + 1;
	reader_op_list = (OP *)malloc((reader_op_list_length + source_op_list_length) * sizeof(OP));
	for(idx = earliest_conflict_seq_no_reader, i = 0; idx <= latest_seq; ++idx, ++i){
		status = readFromWooF(woof_name, (void *)&reader_op_list[i], idx);
		if(status < 0) return 0;
	}

	merged_list_length = 
		mergeOPList(reader_op_list, reader_op_list_length,
			source_op_list, source_op_list_length, (void *)&first_remote_insertion);
	free(source_op_list);

	temp = CLIENT_get_local_woof_name(c, OPERATION_WOOF_NAME);
	memcpy(woof_name, temp, MAX_SIZE_WOOF_NAME);
	free(temp);
	latest_seq = getLatestSeqNo(woof_name);
	if(latest_seq == (unsigned long)-1) return 0;
	if(earliest_conflict_seq_no_reader == (latest_seq + 1)){//no need to prune, just replay the operation
		for(idx = 0; idx < merged_list_length; ++idx){
			if(reader_op_list[idx].op == 1){//insert
				(*insertFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}else if(reader_op_list[idx].op == 0){//delete
				(*deleteFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}
			if(SYNC_REC_TEST && (idx + 1) == SYNC_REC_TEST_VAL){
				fprintf(stdout, "found break - 1\n");
				fflush(stdout);
				return 0;
			}
		}
		return 1;
	}else if((first_remote_insertion + earliest_conflict_seq_no_reader) > latest_seq){//replay from first_remote_insertion
		for(idx = first_remote_insertion; idx < merged_list_length; ++idx){
			if(reader_op_list[idx].op == 1){//insert
				(*insertFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}else if(reader_op_list[idx].op == 0){//delete
				(*deleteFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}
			if(SYNC_REC_TEST && (idx + 1) == SYNC_REC_TEST_VAL){
				fprintf(stdout, "found break\n");
				fflush(stdout);
				return 0;
			}
		}
		return 1;
	}else{
		status = readFromWooF(woof_name, (void *)&op, first_remote_insertion + earliest_conflict_seq_no_reader);//local op woof
		if(status < 0) return 0;

		prune_ht = HT_init(HT_SIZE, MAX_SIZE_WOOF_NAME, sizeof(unsigned long));

		latest_seq = getLatestSeqNo(CONFIG_WOOF_NAME);
		if(latest_seq == (unsigned long)-1) return 0;
		status = readFromWooF(CONFIG_WOOF_NAME, (void *)&config, latest_seq);
		if(status < 0) return 0;
		cp_prune_seq_no = get_CP_seq_no_from_VS(op.vs, config);
		if(cp_prune_seq_no == 0) return 0;//checkpoint does not exist

		//prune CP/AP maps
		status = readFromWooF(CHECKPOINT_WOOF_NAME, (void *)&cp, cp_prune_seq_no);
		if(status < 0) return 0;
		idx = getAPPosFromVS(cp.vs, &failed);
		if(failed) return 0;
		latest_seq = getLatestSeqNo(OPERATION_WOOF_NAME);
		if(latest_seq == (unsigned long)-1) return 0;
		for( ; idx <= latest_seq; ++idx){
			/* get op (needed for vs) */
			status = readFromWooF(OPERATION_WOOF_NAME, (void *)&op_temp, idx);
			if(status < 0) return 0;
			/* populate cp and ap woof names */
			populateCPWooFName(op_temp.vs.nodeID, cp_woof_name);
			populateAPWooFName(op_temp.vs.nodeID, ap_woof_name);
			/* get latest seq no of cp/ap to bound iteration */
			latest_seq_cp_map = getLatestSeqNo(cp_woof_name);
			if(latest_seq_cp_map == (unsigned long)-1) return 0;
			latest_seq_ap_map = getLatestSeqNo(ap_woof_name);
			if(latest_seq_ap_map == (unsigned long)-1) return 0;
			/* prune cp map */
			for(idx_cp_map = op_temp.vs.counter; idx_cp_map <= latest_seq_cp_map; ++idx_cp_map){
				status = readFromWooF(cp_woof_name, (void *)&vsidx, idx_cp_map);
				if(status < 0) return 0;
				if(VS_cmp(vsidx.vs, op_temp.vs) == 0){
					status = pruneWooFs(cp_woof_name, sizeof(VSIDX), AP_WOOF_SIZE, idx_cp_map, base);
					if(status == 0) return 0;
					break;
				}
			}
			/* prune ap map */
			for(idx_ap_map = op_temp.vs.counter; idx_ap_map <= latest_seq_ap_map; ++idx_ap_map){
				status = readFromWooF(ap_woof_name, (void *)&vsidx, idx_ap_map);
				if(status < 0) return 0;
				if(VS_cmp(vsidx.vs, op_temp.vs) == 0){
					status = pruneWooFs(ap_woof_name, sizeof(VSIDX), AP_WOOF_SIZE, idx_ap_map, base);
					if(status == 0) return 0;
					break;
				}
			}
		}
		
		latest_seq = getLatestSeqNo(CHECKPOINT_WOOF_NAME);
		if(latest_seq == (unsigned long)-1) return 0;

		for(idx = cp_prune_seq_no; idx <= latest_seq; ++idx){
			memset((void *)&cp, 0, sizeof(CP));
			status = readFromWooF(CHECKPOINT_WOOF_NAME, (void *)&cp, idx);
			if(status < 0) return 0;
			for(i = 0; i < CPR_PER_CP; ++i){
				if(cp.cpr[i].latest_seq_no == 0) break;
				if(HT_get(prune_ht, (void *)cp.cpr[i].WooFName) == NULL){
					HT_put(prune_ht, (void *)cp.cpr[i].WooFName, (void *)&cp.cpr[i].latest_seq_no);
				}
			}
		}

		for(i = 0; i < HT_SIZE; ++i){
			if(prune_ht->table[i] != NULL){
				slot = prune_ht->table[i];
				while(slot != NULL){
					if(strcmp((char *)slot->key, DATA_WOOF_NAME) == 0){
						element_size = sizeof(DATA);
						history_size = DATA_WOOF_SIZE;
					}else if(strcmp((char *)slot->key, AP_WOOF_NAME) == 0){
						element_size = sizeof(AP);
						history_size = AP_WOOF_SIZE;
					}else{
						element_size = sizeof(LINK);
						history_size = LINK_WOOF_SIZE;
					}
					status = pruneWooFs((char *)slot->key, element_size, history_size, *(unsigned long *)slot->value, base);
					if(status == 0) return 0;
					slot = slot->next;
				}
			}
		}
		//prune everything from cp_prune_seq_no to last CP
		element_size = sizeof(CP);
		history_size = AP_WOOF_SIZE;
		status = pruneWooFs(CHECKPOINT_WOOF_NAME, element_size, history_size, cp_prune_seq_no, base);
		if(status == 0) return 0;
		//prune operation WooF
		element_size = sizeof(OP);
		history_size = AP_WOOF_SIZE;
		status = pruneWooFs(OPERATION_WOOF_NAME, element_size, history_size, first_remote_insertion + earliest_conflict_seq_no_reader, base);
		if(status == 0) return 0;
		//replay [first_remote_insertion, merged_list_length)
		for(idx = first_remote_insertion; idx < merged_list_length; ++idx){
			if(reader_op_list[idx].op == 1){//insert
				(*insertFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}else if(reader_op_list[idx].op == 0){//delete
				(*deleteFunction)(reader_op_list[idx].di, reader_op_list[idx].vs);
			}
			if(SYNC_REC_TEST && (idx - first_remote_insertion + 1) == SYNC_REC_TEST_VAL){
				fprintf(stdout, "found break - 3\n");
				fflush(stdout);
				return 0;
			}
		}

		/*this block was commented out??*/
		strcpy(backup_folder, base);
		strcat(backup_folder, "/");
		strcat(backup_folder, BACKUP_FOLDER_NAME);
		strcpy(delete_folder, base);
		strcat(delete_folder, "/");
		strcat(delete_folder, DELETE_FOLDER_NAME);
		dir = opendir(backup_folder);
		if(dir){
			closedir(dir);
			rename(backup_folder, delete_folder);
			dir = opendir(delete_folder);
			while((de = readdir(dir)) != NULL){
				if((strcmp(de->d_name, ".") != 0) && (strcmp(de->d_name, "..") != 0)){
					strcpy(file_to_be_deleted, delete_folder);
					strcat(file_to_be_deleted, "/");
					strcat(file_to_be_deleted, de->d_name);
					remove(file_to_be_deleted);
				}
			}
			closedir(dir);
			remove(delete_folder);
		}

		return 1;
	}//end of else

	return 0;
}
