#ifndef NODE_H
#define NODE_H

#include <stdbool.h>

#define NODE_STR_SIZE 50

struct Node{
	unsigned long dw_seq_no;
	unsigned long lw_seq_no;
};

typedef struct Node NODE;

char *NODE_str(NODE node);

NODE NODE_failed_op();

bool NODE_is_equal(NODE a, NODE b);

#endif
