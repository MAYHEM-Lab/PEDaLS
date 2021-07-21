#include <stdlib.h>
#include <string.h>

#include "Stack.h"

void StackNode_destroy(StackNode *stackNode){
	
	free(stackNode->element);
	free(stackNode);
	stackNode = NULL;

}

Stack *Stack_init(int element_size){

	Stack *stack;

	stack = (Stack *)malloc(sizeof(Stack));
	stack->head = NULL;
	stack->element_size = element_size;

	return stack;

}

void Stack_push(Stack *stack, void *element){
	
	StackNode *stackNode;

	stackNode = (StackNode *)malloc(sizeof(StackNode));
	stackNode->element = (char *)malloc(stack->element_size * sizeof(char));
	memcpy(stackNode->element, element, stack->element_size);
	stackNode->next = stack->head;
	stack->head = stackNode;

}

StackNode *Stack_pop(Stack *stack){

	StackNode *stackNode;

	stackNode = stack->head;
	
	if(stack->head != NULL) stack->head = stack->head->next;

	return stackNode;

}

void Stack_destroy(Stack *stack){

	StackNode *stackNode;

	while(stack->head != NULL){
		stackNode = stack->head;
		stack->head = stack->head->next;
		StackNode_destroy(stackNode);
	}

	free(stack);
	stack = NULL;

}
