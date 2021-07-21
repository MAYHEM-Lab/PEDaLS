#ifndef STACK_H
#define STACK_H

struct StackNode{
	void *element;
	struct StackNode *next;
};

typedef struct StackNode StackNode;

void StackNode_destroy(StackNode *stackNode);

struct Stack{
	StackNode *head;
	int element_size;
};

typedef struct Stack Stack;

Stack *Stack_init(int element_size);
void Stack_push(Stack *stack, void *element);
StackNode *Stack_pop(Stack *stack);
void Stack_destroy(Stack *stack);

#endif
