#include <stdlib.h>
#include <string.h>

#include "DoublyLinkedList.h"

void DLLNode_destroy(DLLNode *dll_node){
	
	free(dll_node->element);
	free(dll_node);

}

DLL *DLL_init(int element_size){

	DLL *dll;

	dll = (DLL *)malloc(sizeof(DLL));
	dll->head = NULL;
	dll->tail = NULL;
	dll->element_size = element_size;

	return dll;

}

void DLL_insert(DLL *dll, void *element){

	DLLNode *dll_node;

	dll_node = (DLLNode *)malloc(sizeof(DLLNode));
	dll_node->element = malloc(dll->element_size);
	memcpy(dll_node->element, element, dll->element_size);
	dll_node->next = NULL;
	dll_node->prev = dll->tail;

	(dll->head == NULL) ? (dll->head = dll_node) : (dll->tail->next = dll_node);

	dll->tail = dll_node;

}

void DLL_insert_after(DLL *dll, DLLNode *prev_node, void *element){

	DLLNode *dll_node;

	if(prev_node == NULL){
		DLL_insert(dll, element);
		return;
	}

	dll_node = (DLLNode *)malloc(sizeof(DLLNode));
	dll_node->element = malloc(dll->element_size);
	memcpy(dll_node->element, element, dll->element_size);

	if(prev_node == NULL){
		dll_node->next = dll->head;
		dll_node->prev = NULL;
		dll->head = dll_node;
	}else{
		dll_node->prev = prev_node;
		dll_node->next = prev_node->next;
		if(prev_node->next != NULL) prev_node->next->prev = dll_node;
		prev_node->next = dll_node;
		if(dll->tail == prev_node) dll->tail = dll_node;
	}

}

void DLL_insert_before(DLL *dll, DLLNode *next_node, void *element){

	DLLNode *dll_node;

	if(next_node == NULL){
		DLL_insert(dll, element);
		return;
	}

	dll_node = (DLLNode *)malloc(sizeof(DLLNode));
	dll_node->element = malloc(dll->element_size);
	memcpy(dll_node->element, element, dll->element_size);

	if(next_node == NULL){
		dll_node->next = NULL;
		dll_node->prev = dll->tail;
		dll->tail = dll_node;
	}else{
		dll_node->next = next_node;
		dll_node->prev = next_node->prev;
		if(next_node->prev != NULL) next_node->prev->next = dll_node;
		next_node->prev = dll_node;
		if(dll->head == next_node) dll->head = dll_node;
	}

}

void DLL_destroy(DLL *dll){

	DLLNode *dll_node;

	while(dll->head != NULL){
		dll_node = dll->head;
		dll->head = dll->head->next;
		DLLNode_destroy(dll_node);
	}

	free(dll);

}
