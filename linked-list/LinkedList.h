#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>

#include "AccessPointer.h"
#include "Accumulator.h"
#include "Config.h"
#include "DataItem.h"
#include "HashTable.h"
#include "Link.h"
#include "Options.h"
#include "Stack.h"
#include "VersionStamp.h"

extern CONFIG config;
extern HT *cp_ht;

/**
 * initializes the linked list
 *
 * num_of_extra_links: number of extra links per node for persistent version
 * data_woof_size: history size of data woof
 * link_woof_size: history size of link woof
 * ap_woof_size: history size of ap woof
 *
 * returns 1 if initialization successful, 0 otherwise
 **/
int LL_init(
	int num_of_extra_links,
	unsigned long data_woof_size,
	unsigned long link_woof_size,
	unsigned long ap_woof_size
);

///**
// * searches for data item in a given version (persistent) or in the latest version (ephemeral)
// *
// * di: data item to be searched
// * version_stamp: version at which di is searched (ignored for ephemeral)
// *
// * returns the seq. no. in data woof of di if found, 0 if not found, unsigned long representation of -1 if op fails
// **/
//unsigned long LL_search(DI di, unsigned long version_stamp);
//
/**
 * inserts data item into the linked list
 *
 * di: data item to be inserted
 *
 * returns new vs if insertion successful, invalid vs otherwise
 **/
VS LL_insert(DI di, VS vs);

/**
 * deletes data item from the linked list
 *
 * di: data item to be deleted
 *
 * returns new vs if deletion successful, invalid vs otherwise
 **/
VS LL_delete(DI di, VS vs);

/**
 * prints the linked list at a given version (persistent) or the latest version (ephemeral)
 *
 * vs: the version which is printed (ignored for ephemeral)
 *
 * returns 1 if successful, 0 otherwise
 **/
int LL_print(VS vs);

/**
 * dumps debug info
 **/
void LL_debug();

void LL_traverse(int *num);

#endif
