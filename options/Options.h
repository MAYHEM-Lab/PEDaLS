#ifndef OPTIONS_H
#define OPTIONS_H

#define MAX_SIZE_WOOF_NAME 256		// maximum length for a WooF name
#define NUM_RETRIES 3 				// number of retries for remote calls

#define INVALID_COUNTER 0			// invalid counter, used to signal node not found
#define INVALID_NODE_ID '?'			// invalid node id, used to signal node not found

#define REPLICATED 0				// set to 1 for replicated structures
#define NUM_OF_EXTRA_LINKS 1		// number of extra links, 0 indicates ephemeral structure

#define HT_SIZE 769					// hash table size for in-memory usage

#define IP_ADDR_SIZE 16				// ip address length
#define IP_TABLE_SIZE 128			// maximum length of IP table, i.e., number of nodes

#define SYNC_CODE 2					// code for synchronization
#define INSERT_CODE 1				// code for insertion
#define DELETE_CODE 0				// code for deletion

#define TOP_FNAME "topology.txt"	// topology filename
#define BACKUP_FOLDER_NAME "backup"
#define DELETE_FOLDER_NAME "tobedeleted"

#define AP_WOOF_NAME "AP"
#define DATA_WOOF_NAME "DATA"
#define CONFIG_WOOF_NAME "CONFIG"
#define CHECKPOINT_WOOF_NAME "CHECKPOINT"
#define MAX_COUNTER_WOOF_NAME "MAX_COUNTER"
#define OPERATION_WOOF_NAME "OPERATION"
#define REMOTE_SEQUENCER_WOOF_NAME "REMOTE_SEQUENCER"
#define SEQUENCER_WOOF_NAME "SEQUENCER"
#define SEQUENCER_COMPLETION_WOOF_NAME "SEQUENCER_COMPLETION"
#define TOPOLOGY_NODE_WOOF_NAME "TOPOLOGY_NODE"
#define PROGRESS_STATUS_WOOF_NAME "PROGSTAT"
#define TEMP_WOOF_NAME "TEMP"

#define AP_WOOF_SIZE 1000
#define DATA_WOOF_SIZE 1000
#define LINK_WOOF_SIZE 3000
#define CONFIG_WOOF_SIZE 1
#define CHECKPOINT_WOOF_SIZE AP_WOOF_SIZE
#define OPERATION_WOOF_SIZE AP_WOOF_SIZE
#define SEQUENCER_WOOF_SIZE AP_WOOF_SIZE
#define MAX_COUNTER_WOOF_SIZE 1

#endif
