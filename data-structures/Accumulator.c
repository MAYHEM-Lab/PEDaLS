#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Accumulator.h"

void AccumulatorNode_destroy(AccumulatorNode *accumulatorNode){
	
	free(accumulatorNode->element);
	free(accumulatorNode);

}

Accumulator *Accumulator_init(int element_size){

	Accumulator *accumulator;

	accumulator = (Accumulator *)malloc(sizeof(Accumulator));
	accumulator->head = NULL;
	accumulator->tail = NULL;
	accumulator->element_size = element_size;

	return accumulator;

}

void Accumulator_insert(Accumulator *accumulator, void *element){
	
	AccumulatorNode *accumulatorNode;

	accumulatorNode = (AccumulatorNode *)malloc(sizeof(AccumulatorNode));
	accumulatorNode->element = (char *)malloc(accumulator->element_size * sizeof(char));
	memcpy(accumulatorNode->element, element, accumulator->element_size);
	accumulatorNode->next = NULL;
	if(accumulator->head == NULL){
		accumulator->head = accumulatorNode;
		accumulator->tail = accumulatorNode;
	}else{
		accumulator->tail->next = accumulatorNode;
		accumulator->tail = accumulatorNode;
	}

}

void Accumulator_destroy(Accumulator *accumulator){

	AccumulatorNode *accumulatorNode;

	while(accumulator->head != NULL){
		accumulatorNode = accumulator->head;
		accumulator->head = accumulator->head->next;
		AccumulatorNode_destroy(accumulatorNode);
	}

	free(accumulator);

}

bool Accumulator_is_equal(Accumulator *a, Accumulator *b){

	AccumulatorNode *node_a;
	AccumulatorNode *node_b;

	if(a->element_size != b->element_size) return false;

	node_a = a->head;
	node_b = b->head;

	while(node_a != NULL && node_b != NULL){
		if(memcmp(node_a->element, node_b->element, a->element_size) != 0) return false;
		node_a = node_a->next;
		node_b = node_b->next;
	}

	if(node_a != NULL || node_b != NULL) return false;

	return true;

}
