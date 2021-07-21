#ifndef CSPOT_HELPERS_H
#define CSPOT_HELPERS_H

#include "HashTable.h"

/**
 * returns < 0 on failure
 **/
int createWooF(char *woof_name, unsigned long element_size, unsigned long history_size);

/**
 * returns woof seq no if successful
 * 		   unsigned long -1 if fails
 **/
unsigned long insertIntoWooF(char *woof_name, char *handler_name, void *element, HT *cp);

/**
 * returns < 0 on failure
 **/
int readFromWooF(char *woof_name, void *element, unsigned long seq_no);

/**
 * returns last seq no if successful
 * 		   unsigned long -1 if fails
 **/
unsigned long getLatestSeqNo(char *woof_name);

/**
 * populates latest entry of woof_name in element
 *
 * returns 1 on success
 * returns -1 on failure
 * returns 0 if empty
 **/
int getLastEntry(char *woof_name, void *element);

/**
 * dumps woof
 **/
void dumpWooF(char *woof_name, int element_size, char *tag);

/**
 * compute space
 **/
unsigned long computeSpace(bool replicated, char *top_fname);

#endif
