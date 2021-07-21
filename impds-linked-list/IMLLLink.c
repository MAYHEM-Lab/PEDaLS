#include <stdlib.h>

#include "IMLLLink.h"
#include "pmalloc.h"
#include "IMLLOptions.h"

IMLLLINK *IMLLLINK_create(unsigned long version_stamp, IMLLNODE *to, int memory_mapped){

	IMLLLINK *link;

	if(memory_mapped){
		link = (IMLLLINK *)Pmalloc(sizeof(IMLLLINK));
	}else{
		link = (IMLLLINK *)malloc(sizeof(IMLLLINK));
	}
	SPACE += sizeof(IMLLLINK);
	link->version_stamp = version_stamp;
	link->to = to;

	if(memory_mapped){
		PmallocSyncObject((unsigned char *)link, sizeof(IMLLLINK));
	}

	return link;

}

