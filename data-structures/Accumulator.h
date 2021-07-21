#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <stdbool.h>

struct AccumulatorNode{
	void *element;
	struct AccumulatorNode *next;
};

typedef struct AccumulatorNode AccumulatorNode;

void AccumulatorNode_destroy(AccumulatorNode *accumulatorNode);

struct Accumulator{
	AccumulatorNode *head;
	AccumulatorNode *tail;
	int element_size;
};

typedef struct Accumulator Accumulator;

Accumulator *Accumulator_init(int element_size);
void Accumulator_insert(Accumulator *accumulator, void *element);
void Accumulator_destroy(Accumulator *accumulator);

bool Accumulator_is_equal(Accumulator *a, Accumulator *b);

#endif
