#ifndef GENERIC_HELPERS_H
#define GENERIC_HELPERS_H

#include "VersionStamp.h"

/**
 * returns 1 if successful, 0 otherwise
 **/
int populateIPAddress(char *IP);

/**
 * returns node id corresponding to machine IP
 * '?' if fails
 **/
char getNodeIDFromTOP(char *filename);

/**
 * generates link woof name from vs
 **/
char *getLinkWooFName(VS working_vs);

#endif
