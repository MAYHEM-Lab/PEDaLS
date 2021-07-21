#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "Options.h"
#include "VersionStamp.h"

#define CPR_PER_CP 5
#define CPR_STR_SIZE 300
#define CP_STR_SIZE_EXTRA 50

struct CheckpointRecord{
	char WooFName[MAX_SIZE_WOOF_NAME];
	unsigned long latest_seq_no;
};

typedef struct CheckpointRecord CPR;

char *CPR_str(CPR cpr);

struct Checkpoint{
	VS vs;
	CPR cpr[CPR_PER_CP];
};

typedef struct Checkpoint CP;

char *CP_str(CP cp);

#endif
