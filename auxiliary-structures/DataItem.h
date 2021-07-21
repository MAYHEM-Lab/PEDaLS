#ifndef DATA_ITEM_H
#define DATA_ITEM_H

#define DI_STR_SIZE 40

struct DataItem{
	int val;
};

typedef struct DataItem DI;

/**
 * returns a string representation of data item
 **/
char *DI_str(DI di);

/**
 * compares two data items
 *
 * di1: first data item
 * di2: second data item
 *
 * returns:
 * 	-1 if di1 < di2
 * 	0 if di1 == di2
 * 	1 if di1 > di2
 **/
int DI_cmp(DI di1, DI di2);

#endif
