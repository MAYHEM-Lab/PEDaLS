#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Topology.h"
#include "Options.h"

TOP *getTopology(char *filename){

	FILE *fh;
	int numNodes;
	TOP *top;
	int i;
	char nodeID;
	char IP[IP_ADDR_SIZE];

	fh = fopen(filename, "r");
	if(fh == NULL){
		fprintf(stderr, "could not open topology file: %s\n", filename);
		fflush(stderr);
		return NULL;
	}

	top = (TOP *)malloc(sizeof(TOP));
	top->IPTable = (char **)malloc(IP_TABLE_SIZE * sizeof(char *));

	fscanf(fh, "%d\n", &numNodes);
	top->numNodes = numNodes;
	top->nodeIDs = (char *)malloc(numNodes * sizeof(char));

	for(i = 0; i < numNodes; ++i){
		fscanf(fh, "%c,", &nodeID);
		memset(IP, 0, sizeof(IP));
		fscanf(fh, "%s\n", IP);
		top->IPTable[(int)nodeID] = (char *)malloc(IP_ADDR_SIZE * sizeof(char));
		strcpy(top->IPTable[(int)nodeID], IP);
		top->nodeIDs[i] = nodeID;
	}

	return top;

}

char getNodeIDFromTOPandIP(TOP *top, char *ip_addr){

	char nodeID;
	int i;

	nodeID = '?';

	for(i = 0; i < top->numNodes; ++i){
		if(strcmp(top->IPTable[(int)top->nodeIDs[i]], ip_addr) == 0){
			nodeID = top->nodeIDs[i];
			break;
		}
	}

	return nodeID;

}
