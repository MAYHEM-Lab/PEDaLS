#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "AccessPointer.h"
#include "Checkpoint.h"
#include "CSPOTHelpers.h"
#include "Data.h"
#include "Knowledge.h"
#include "Link.h"
#include "Options.h"
#include "Topology.h"
#include "VersionStampIdxMap.h"

#include "woofc.h"
#include "woofc-host.h"

int createWooF(char *woof_name, unsigned long element_size, unsigned long history_size){
    int err;
	int i;
	
	for(i = 0; i < NUM_RETRIES; ++i){
		err = WooFCreate(woof_name, element_size, history_size);
		if(err < 0){
			fprintf(stderr, "could not create WooF named %s\n", woof_name);
			fflush(stderr);
		}else{
			break;
		}
	}
    return err;
}

unsigned long insertIntoWooF(char *woof_name, char *handler_name, void *element, HT *cp){
	int i;
	unsigned long idx;
	CPR cpr;

	for(i = 0; i < NUM_RETRIES; ++i){
		idx = WooFPut(woof_name, handler_name, element);
		if(WooFInvalid(idx)) {
			fprintf(stderr,"WooFPut failed for %s\n",woof_name);
			fflush(stderr);
		}else{
			if(cp != NULL){
				memset((void *)&cpr, 0, sizeof(CPR));
				strncpy(cpr.WooFName, woof_name, MAX_SIZE_WOOF_NAME);
				cpr.latest_seq_no = idx;
				HT_put(cp, (void *)&cpr.WooFName, (void *)&cpr.latest_seq_no);
			}
			break;
		}
	}

	return idx;

}

int readFromWooF(char *woof_name, void *element, unsigned long seq_no){
	int status;
	int i;

	for(i = 0; i < NUM_RETRIES; ++i){
		status = WooFGet(woof_name, element, seq_no);
		if(status < 0) {
			fprintf(stderr,"WooFGet failed for %s at seq_no %lu\n",woof_name, seq_no);
			fflush(stderr);
		}else{
			break;
		}
	}

	return status;
}

unsigned long getLatestSeqNo(char *woof_name){
	int i;
	unsigned long seq_no;

	for(i = 0; i < NUM_RETRIES; ++i){
		seq_no = WooFGetLatestSeqno(woof_name);
		if(seq_no == (unsigned long)-1){
			fprintf(stderr,"could not get latest seq no of %s\n",woof_name);
			fflush(stderr);
		}else{
			return seq_no;
		}
	}

	return (unsigned long)-1;
}

int getLastEntry(char *woof_name, void *element){
	unsigned long latest_seq;
	int status;

	latest_seq = getLatestSeqNo(woof_name);
	if(latest_seq == (unsigned long)-1) return -1;
	if(latest_seq == 0) return 0;
	status = readFromWooF(woof_name, element, latest_seq);
	if(status < 0) return -1;
	return 1;
}

void dumpWooF(char *woof_name, int element_size, char *tag){

	unsigned long latest_seq;
	unsigned long idx;
	void *element;

	element = malloc(element_size);

	fprintf(stdout, "dumping %s\n", woof_name);

	latest_seq = getLatestSeqNo(woof_name);
	for(idx = 1; idx <= latest_seq; ++idx){
		readFromWooF(woof_name, element, idx);
		if(strcmp(tag, "LINK") == 0) fprintf(stdout, "%s\n", LINK_str(*(LINK *)element));
		else if(strcmp(tag, "NODE") == 0) fprintf(stdout, "%s\n", NODE_str(*(NODE *)element));
		else if(strcmp(tag, "AP") == 0) fprintf(stdout, "%s\n", AP_str(*(AP *)element));
		else if(strcmp(tag, "DATA") == 0) fprintf(stdout, "%s\n", DATA_str(*(DATA *)element));
		else if(strcmp(tag, "APSUB") == 0) fprintf(stdout, "%s\n", VSIDX_str(*(VSIDX *)element));
	}

	fflush(stdout);

	free(element);

}

unsigned long computeSpace(bool replicated, char *top_fname){

	unsigned long space;
	unsigned long idx;
	DATA data;
	int status;
	unsigned long i;
	unsigned long latest_seq;
	TOP *top;
	char nodeID;
	char ap_woof_name[MAX_SIZE_WOOF_NAME];
	char cp_woof_name[MAX_SIZE_WOOF_NAME];
	char k_woof_name[MAX_SIZE_WOOF_NAME];

	space = 0;

	/* AP WooF */
	idx = getLatestSeqNo(AP_WOOF_NAME);
	if(idx == (unsigned long)-1) return (unsigned long)-1;
	space += (idx * sizeof(AP));

	/* DATA WooF */
	idx = getLatestSeqNo(DATA_WOOF_NAME);
	if(idx == (unsigned long)-1) return (unsigned long)-1;
	space += (idx * sizeof(DATA));

	/* LINK WooF */
	for(i = 1; i <= idx; ++i){
		status = readFromWooF(DATA_WOOF_NAME, (void *)&data, i);
		if(status < 0) return (unsigned long)-1;
		latest_seq = getLatestSeqNo(data.lw_name);
		if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
		space += (latest_seq * sizeof(LINK));
	}

	if(! replicated) return space;

	/* CP WooF */
	latest_seq = getLatestSeqNo(CHECKPOINT_WOOF_NAME);
	if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
	space += (latest_seq * sizeof(CP));

	top = getTopology(top_fname);
	for(i = 0; i < top->numNodes; ++i){
		nodeID = top->nodeIDs[i];
		/* KNOW WooF */
		memset(k_woof_name, 0, MAX_SIZE_WOOF_NAME);
		k_woof_name[0] = nodeID;
		latest_seq = getLatestSeqNo(k_woof_name);
		if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
		space += (latest_seq * sizeof(KNOW));
		/* APX WooF */
		memset(ap_woof_name, 0, MAX_SIZE_WOOF_NAME);
		sprintf(ap_woof_name, "%s%c", AP_WOOF_NAME, nodeID);
		latest_seq = getLatestSeqNo(ap_woof_name);
		if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
		space += (latest_seq * sizeof(VSIDX));
		/* CHECKPOINTX WooF */
		memset(cp_woof_name, 0, MAX_SIZE_WOOF_NAME);
		sprintf(cp_woof_name, "%s%c", CHECKPOINT_WOOF_NAME, nodeID);
		latest_seq = getLatestSeqNo(cp_woof_name);
		if(latest_seq == (unsigned long)-1) return (unsigned long)-1;
		space += (latest_seq * sizeof(VSIDX));
	}

	return space;

}
