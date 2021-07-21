#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HashTable.h"

SLOT *SLOT_create(void *key, void *value, int key_size, int value_size){
	
	SLOT *slot;

	slot = (SLOT *)malloc(sizeof(SLOT));
	memset((void *)slot, 0, sizeof(SLOT));

	slot->key = malloc(key_size * sizeof(char));
	memset(slot->key, 0, key_size);
	slot->value = malloc(value_size * sizeof(char));
	memset(slot->value, 0, value_size);

	memcpy(slot->key, key, key_size);
	memcpy(slot->value, value, value_size);
	slot->next = NULL;

	return slot;

}

void SLOT_destroy(SLOT *slot){

	SLOT *current;
	SLOT *next;

	current = slot;

	while(current){
		next = current->next;

		free(current->key);
		free(current->value);
		current->key = NULL;
		current->value = NULL;

		free(current);

		current = next;
	}

}

HT *HT_init(int ht_size, int key_size, int value_size){

	HT *ht;

	ht = (HT *)malloc(sizeof(HT));
	memset((void *)ht, 0, sizeof(HT));

	ht->key_size = key_size;
	ht->value_size = value_size;
	ht->ht_size = ht_size;
	ht->table = (SLOT **)malloc(ht_size * sizeof(SLOT *));

	//for(i = 0; i < ht_size; ++i){
	//	ht->table[i] = NULL;
	//}
	memset(ht->table, 0, ht_size * sizeof(SLOT *));

	return ht;

}

int HT_hash(HT *ht, void *key){

	int i;
	char *val;
	unsigned long hash_val;

	val = (char *)malloc(ht->key_size * sizeof(char));
	memcpy(val, key, ht->key_size);
	hash_val = 5381;
	for(i = 0; i < ht->key_size; ++i){
		hash_val = ((hash_val << 5) + hash_val) + val[i];
	}

	free(val);

	return (hash_val % ht->ht_size);

}

int HT_put(HT *ht, void *key, void *value){

	int pos;
	SLOT *slot;
	SLOT *iterator;
	int key_size;
	int value_size;

	slot = NULL;
	iterator = NULL;

	key_size = ht->key_size;
	value_size = ht->value_size;

	pos = HT_hash(ht, key);
	iterator = ht->table[pos];
	slot = SLOT_create(key, value, key_size, value_size);

	if(iterator == NULL){
		//ht->table[pos] = (SLOT *)malloc(sizeof(SLOT));
		//memcpy(ht->table[pos], slot, sizeof(SLOT));
		ht->table[pos] = slot;
	}else{
		//slot = SLOT_create(key, value, key_size, value_size);
		while(1){
			if(HT_equal_key(ht, key, iterator->key)){
				memcpy(iterator->value, slot->value, ht->value_size);
				SLOT_destroy(slot);
				break;
			}else if(iterator->next == NULL){
				iterator->next = slot;
				break;
			}else{
				iterator = iterator->next;
			}
		}
	}

	return pos;

}

void *HT_get(HT *ht, void *key){
	
	SLOT *iterator;
	int pos;

	iterator = NULL;

	pos = HT_hash(ht, key);
	iterator = ht->table[pos];

	if(iterator == NULL){
		return NULL;
	}else{
		while(iterator != NULL){
			if(HT_equal_key(ht, key, iterator->key)){
				return iterator->value;
			}
			iterator = iterator->next;
		}
	}

	return NULL;

}

bool HT_equal_key(HT *ht, void *key1, void *key2){

	char *val1;
	char *val2;
	int i;

	val1 = (char *)key1;
	val2 = (char *)key2;

	for(i = 0; i < ht->key_size; ++i){
		if(val1[i] != val2[i]) return false;
	}

	return true;

}

bool HT_exists(HT *ht, void *key){
	
	int pos;
	SLOT *iterator;

	pos = HT_hash(ht, key);
	iterator = ht->table[pos];

	if(iterator == NULL) return false;

	while(iterator != NULL){
		if(HT_equal_key(ht, key, iterator->key)) return true;
		iterator = iterator->next;
	}

	return false;

}

void HT_destroy(HT *ht){

	int i;

	for (i = 0; i < ht->ht_size; ++i){
		if(ht->table[i] != NULL){
			SLOT_destroy(ht->table[i]);
		}
	}

	free(ht->table);
	
	free(ht);
	ht = NULL;

}
