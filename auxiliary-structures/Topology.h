#ifndef TOPOLOGY_H
#define TOPOLOGY_H

struct Topology{
	int numNodes;
	char **IPTable;
	char *nodeIDs;
};

typedef struct Topology TOP;

/**
 * expects a file with one or more lines
 * in each line there are two comma separated values
 * the first value is a single character, the node ID
 * the second value is an IP address in XXX.XXX.XXX.XXX format
 **/
TOP *getTopology(char *filename);

/**
 * returns node id of an ip address in topology
 *
 * @param top: topology
 * @param ip_addr: ip address
 *
 * returns ? if not found, otherwise the correct node if
 **/
char getNodeIDFromTOPandIP(TOP *top, char *ip_addr);

#endif
