#ifndef REPLICATION_HELPERS_H
#define REPLICATION_HELPERS_H

#include "Client.h"
#include "Config.h"
#include "HashTable.h"
#include "Operation.h"
#include "Topology.h"
#include "VersionStamp.h"

/**
 * retrieves position of vs in global AP from (vs, idx) sub AP
 *
 * @param vs: vs for which position in global AP is generated
 * @param failed: out param, set to true if op fails
 *
 * returns positiong in global AP on success, (unsigned long)-1 if vs not found in ap map but vs counter is greater than the counter of the last entry in map, sets out param to true if op fails
 **/
unsigned long getAPPosFromVS(VS vs, bool *failed);

/**
 * checkpoints data/link/ap/apmap woofs for given vs
 *
 * @param config: config for current structure
 * @param working_vs: working vs
 * @param cp_ht: hash table containing woof name and index
 *
 * returns the first row in checkpoint woof for the working vs, (unsigned long)-1 on failure
 **/
unsigned long writeCP(CONFIG config, VS working_vs, HT *cp_ht);

/**
 * records the information required for replication before an operation completes:
 * 	-- updates ap map
 * 	-- writes checkpoint
 * 	-- updates cp map
 * 	-- updates op
 * 	-- updates knowledge
 *
 * @param config: config info for structure
 * @param working_vs: working vs
 * @param ap_seq: sequence number of AP for the current op
 * @param op_code: insert or delete from options constants
 * @param di: data item i.e. operation of the operation
 * @param cp_ht: checkpoint info i.e. map of woof name --> idx
 **/
int opLastPhaseReplication(CONFIG config, VS working_vs, unsigned long ap_seq, int op_code, DI di, HT *cp_ht);

/**
 * @param woof_name:
 * @param earliest_conflict_seq_no:
 * @param intended_predecessor:
 **/
int populateIntededPredecessor(char *woof_name, unsigned long earliest_conflict_seq_no, OP *intended_predecessor);

/**
 * @param earliest_conflict_seq_no: out param, earliest sequence number of source where there might be a conflict
 * @param earliest_conflict_seq_no_reader: out param, earliest sequence number of reader where there might be a conflict
 * @param top: topology
 * @param c: client object with reader as local and source as remote
 * @param local_nodeID: node id of reader
 * @param remote_nodeID: node id of source
 *
 * returns 	-1 on failure
 * 			0 if no conflict
 * 			1 if conflict detected
 **/
int detectConflict(
	unsigned long *earliest_conflict_seq_no,
	unsigned long *earliest_conflict_seq_no_reader,
	TOP *top,
	CLIENT c,
	char local_nodeID,
	char remote_nodeID
);

unsigned long get_CP_seq_no_from_VS(VS vs, CONFIG config);

void populateAPWooFName(char nodeID, char *ap_woof_name);

void populateCPWooFName(char nodeID, char *cp_woof_name);

int pruneWooFs(char *woof_name, unsigned long element_size, unsigned long history_size, unsigned long prune_start_seq, char *base);

OP getIntendedPredecessorFromList(OP *op_list, unsigned long idx);

/**
 * returns number of distinct entries in merged list
 **/
unsigned long mergeOPList(
	OP *reader_op_list, 
	unsigned long reader_op_list_length, 
	OP *source_op_list, 
	unsigned long source_op_list_length,
	unsigned long *first_remote_insertion
);

/**
 * returns 0 on failure
 * returns 1 on success
 **/
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
);

#endif
