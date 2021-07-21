#ifndef ACCESS_POINTER_H
#define ACCESS_POINTER_H

#define AP_STR_SIZE 80

#include "VersionStamp.h"
#include "Node.h"

struct AccessPointer{
	VS vs;
	NODE node;
};

typedef struct AccessPointer AP;

char *AP_str(AP ap);

#endif
