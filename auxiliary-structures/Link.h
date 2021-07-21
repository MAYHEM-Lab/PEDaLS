#ifndef LINK_H
#define LINK_H

#include "Node.h"
#include "VersionStamp.h"

#define LINK_STR_SIZE 120

struct Link{

    VS vs; 
    VS dw_vs; 
	NODE node;
	int num_of_remaining_links;
    char type;

};

typedef struct Link LINK;

char *LINK_str(LINK link);

#endif
