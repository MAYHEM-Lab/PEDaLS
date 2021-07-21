#include <stdlib.h>
#include <string.h>

#include "pmalloc.h"
#include "IMBSTOptions.h"
#include "IMBSTStack.h"

IMBSTS *IMBSTS_init(int memory_mapped){
	IMBSTS *stack;
	
	if(memory_mapped){
		stack = (IMBSTS *)Pmalloc(sizeof(IMBSTS));
	}else{
		stack = (IMBSTS *)malloc(sizeof(IMBSTS));
	}
	//SPACE += sizeof(IMBSTS);
	stack->memory_mapped = memory_mapped;
	stack->head = NULL;

	//if(memory_mapped){
	//	PmallocSyncObject((unsigned char *)stack, sizeof(IMBSTS));
	//}

	return stack;
}

void IMBSTS_push(IMBSTS *stack, IMBSTNODE *node){
	IMBSTSN *sn;

	if(stack->memory_mapped){
		sn = (IMBSTSN *)Pmalloc(sizeof(IMBSTSN));
	}else{
		sn = (IMBSTSN *)malloc(sizeof(IMBSTSN));
	}
	//SPACE += sizeof(IMBSTSN);

	sn->node = node;
	sn->next = stack->head;
	stack->head = sn;

	//if(stack->memory_mapped){
	//	PmallocSyncObject((unsigned char *)sn, sizeof(IMBSTSN));
	//}

}

IMBSTSN *IMBSTS_pop(IMBSTS *stack){

	IMBSTSN *sn;

	sn = stack->head;

	if(stack->head != NULL) stack->head = stack->head->next;

	return sn;

}
