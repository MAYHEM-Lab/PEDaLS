#include <arpa/inet.h>
#include <dirent.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GenericHelpers.h"
#include "Options.h"
#include "Topology.h"

int populateIPAddress(char *IP){

	char host[256];
	struct hostent *host_entry;
	int hostname;

	hostname = gethostname(host, sizeof(host));
	if(hostname == -1){
		perror("gethostname");
		return 0;
	}

	host_entry = gethostbyname(host);
	if(host_entry == NULL){
		perror("gethostbyname");
		return 0;
	}

	strcpy(IP, inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])));

	return 1;

}

char getNodeIDFromTOP(char *filename){

	char IP[IP_ADDR_SIZE];
	char nodeID;
	int i;
	TOP *top;

	nodeID = '?';
	populateIPAddress(IP);
	top = getTopology(TOP_FNAME);
	for(i = 0; i < top->numNodes; ++i){
		if(strcmp(top->IPTable[(int)top->nodeIDs[i]], IP) == 0){
			nodeID = top->nodeIDs[i];
			break;
		}
	}

	return nodeID;

}

char *getLinkWooFName(VS working_vs){
	char *result;
	result = (char *)malloc((MAX_SIZE_WOOF_NAME + 1)* sizeof(char));
	sprintf(result, "%lu%c", working_vs.counter, working_vs.nodeID);
	return result;
}
