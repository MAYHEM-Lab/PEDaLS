#ifndef VERSION_STAMP_H
#define VERSION_STAMP_H

#define VS_STR_SIZE 40

struct VersionStamp{
	unsigned long counter;
	char nodeID;
};

typedef struct VersionStamp VS;

/**
 * compares nodeID of two version stamps
 *
 * @param a version stamp
 * @param b version stamp
 *
 * returns -1 if nodeID of a is less than that of b
 * returns 1 if nodeID of a is greater than that of b
 * returns 0 otherwise
 */
int VS_node_id_cmp(VS a, VS b);

/**
 * compares two version stamps
 *
 * @param a version stamp
 * @param b version stamp
 *
 * returns -1 if a is less than b
 * returns 1 if a is greater than b
 * returns 0 otherwise
 */
int VS_cmp(VS a, VS b);

/**
 * returns string representation of version stamp
 */
char *VS_str(VS vs);

/**
 * returns an invalid version stamp
 **/
VS VS_invalid();

#endif
