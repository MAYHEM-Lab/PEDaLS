#ifndef TOPOLOGY_NODE_H
#define TOPOLOGY_NODE_H

#include "Options.h"

#define TOPNODE_STR_SIZE 50

struct TopologyNode{
	char nodeID;
	char IP[IP_ADDR_SIZE];
};

typedef struct TopologyNode TOPNODE;

char *TOPNODE_str(TOPNODE tn);

#endif
