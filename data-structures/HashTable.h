/**
 * for hash functions take a look at
 * http://www.cse.yorku.ca/~oz/hash.html
 * djb2 used here
 **/

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>

struct Slot{
	void *key;
	void *value;
	struct Slot *next;
};

typedef struct Slot SLOT;

/**
 * creates slot element
 *
 * key: key of the slot
 * value: value of the key
 * key_size: size of the key in bytes
 * value_size: size of the value in bytes
 */
SLOT *SLOT_create(void *key, void *value, int key_size, int value_size);

/**
 * destructor for slot
 */
void SLOT_destroy(SLOT *slot);

struct HashTable{
	SLOT **table;
	int key_size;
	int value_size;
	int ht_size;
};

typedef struct HashTable HT;

/**
 * initializes hash table
 *
 * ht_size: size of hash table
 * key_size: size of the key to be hashed
 * value_size: size of the value
 **/
HT *HT_init(int ht_size, int key_size, int value_size);

/**
 * computes hash value i.e. position in table
 *
 * ht: hash table
 * key: key to be hashed
 */
int HT_hash(HT *ht, void *key);

/**
 * inserts key into the hash table 
 * returns pos in hash table, -1 if already present
 * 
 * ht: hash table
 * key: key to be inserted
 * value: valueto be inserted
 */
int HT_put(HT *ht, void *key, void *value);

/**
 * gets value corresponding to the given key
 *
 * ht: hash table
 * key: key to be retrieved
 *
 * returns pointer to data, NULL if not present
 */
void *HT_get(HT *ht, void *key);

/**
 * compares two keys
 * returns true if they are equal, false otherwise
 *
 * ht: hash table
 * key1: first key
 * key2: second key
 */
bool HT_equal_key(HT *ht, void *key1, void *key2);

/**
 * determines whether key is present in hash table
 * returns true if present, false otherwise
 *
 * ht: hash table
 * key: key to be searched
 */
bool HT_exists(HT *ht, void *key);

/**
 * destructor for hash table
 */
void HT_destroy(HT *ht);

#endif
