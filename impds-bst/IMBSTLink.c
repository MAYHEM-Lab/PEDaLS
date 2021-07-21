#include <stdio.h>
#include <stdlib.h>

#include "IMBSTOptions.h"
#include "IMBSTLink.h"
#include "pmalloc.h"

IMBSTLINK *IMBSTLINK_create(unsigned long version_stamp, IMBSTNODE *to, int type, int memory_mapped){
	IMBSTLINK *link;

	if(memory_mapped){
		link = (IMBSTLINK *)Pmalloc(sizeof(IMBSTLINK));
	}else{
		link = (IMBSTLINK *)malloc(sizeof(IMBSTLINK));
	}
	SPACE += sizeof(IMBSTLINK);

	link->version_stamp = version_stamp;
	if(type == 2) fprintf(stdout, "this was not supposed to happen!!!\n");
	link->type = type;
	link->to = to;

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)link, sizeof(IMBSTLINK));
	}

	return link;

}
