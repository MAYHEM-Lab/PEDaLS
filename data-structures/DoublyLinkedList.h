#ifndef DOUBLY_LINKED_LIST
#define DOUBLY_LINKED_LIST

struct DoublyLinkedListNode{
	void *element;
	struct DoublyLinkedListNode *next;
	struct DoublyLinkedListNode *prev;
};

typedef struct DoublyLinkedListNode DLLNode;

void DLLNode_destroy(DLLNode *dll_node);

struct DoublyLinkedList{
	DLLNode *head;
	DLLNode *tail;
	int element_size;
};

typedef struct DoublyLinkedList DLL;

DLL *DLL_init(int element_size);
/* inserts at tail */
void DLL_insert(DLL *dll, void *element);
/* insert after prev_node, if null insert at tail */
void DLL_insert_after(DLL *dll, DLLNode *prev_node, void *element);
/* insert before next_node, if null insert at tail */
void DLL_insert_before(DLL *dll, DLLNode *next_node, void *element);
void DLL_destroy(DLL *dll);

#endif
