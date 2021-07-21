#ifndef OPERATION_H
#define OPERATION_H

#include "DataItem.h"
#include "VersionStamp.h"

#define OP_STR_SIZE 600

struct Operation{
	VS vs;
	int op;
	DI di;
};

typedef struct Operation OP;

/**
 * returns a string representation of operation
 */
char *OP_str(OP op);

#endif
