#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "mio.h"
#include "pmalloc.h"

unsigned long SIZE;

struct Node{
	int val;
	struct Node *next;
	struct Node *prev;
};

typedef struct Node NODE;

NODE *NODE_create(int val){
	NODE *node;

	//node = (NODE *)malloc(sizeof(NODE));
	node = (NODE *)Pmalloc(sizeof(NODE));
	node->val = val;
	node->next = NULL;
	node->prev = NULL;

	SIZE += sizeof(NODE);

	PmallocSyncObject((unsigned char *)node, sizeof(NODE));

	return node;
}

struct LinkedList{
	NODE *head;
};

typedef struct LinkedList LL;

LL *LL_create(){
	LL *ll;

	//ll = (LL *)malloc(sizeof(LL));
	ll = (LL *)Pmalloc(sizeof(LL));
	ll->head = NULL;

	SIZE += sizeof(LL);

	PmallocSyncObject((unsigned char *)ll, sizeof(LL));

	return ll;
}

void LL_insert(LL *ll, int val){
	NODE *iterator;

	if(ll->head == NULL){
		ll->head = NODE_create(val);
		return;
	}

	iterator = ll->head;
	while(iterator->next != NULL){
		iterator = iterator->next;
	}

	iterator->next = NODE_create(val);
	iterator->next->prev = iterator;
}

NODE *search(LL *ll, int val){
	NODE *iterator;

	iterator = NULL;
	if(ll != NULL) iterator = ll->head;
	
	while(1){
		if(iterator == NULL) break;
		if(iterator->val == val) return iterator;
		iterator = iterator->next;
	}

	return iterator;
}

void LL_delete(LL *ll, int val){
	NODE *target;

	target = search(ll, val);
	if(target == NULL) return;

	SIZE -= sizeof(NODE);

	if(target == ll->head){//first element being deleted
		if(target->next != NULL) target->next->prev = NULL;
		ll->head = target->next;
		return;
	}

	target->prev->next = target->next;
	if(target->next != NULL) target->next->prev = target->prev;
}

void LL_print(LL *ll){
	NODE *iterator;

	iterator = ll->head;

	while(iterator != NULL){
		fprintf(stdout, "%d ", iterator->val);
		iterator = iterator->next;
	}

	fprintf(stdout, "\n");
}

void LL_traverse(LL *ll, int *num){

	NODE *iterator;

	*num = 0;

	if(ll == NULL) return;
	iterator = ll->head;

	while(iterator != NULL){
		*num += 1;
		iterator = iterator->next;
	}

}

void run_workload(char *workload, unsigned long *time_arr, unsigned long *space_arr, int code){

	char access_output[256];
	FILE *fp;
	int op;
	int val;
	int i;
	int num;
	int num_ops;
	struct timeval ts_start;
	struct timeval ts_end;
	unsigned long elapsed_time;
	LL *ll;

	fp = fopen(workload, "r");
	fscanf(fp, "%d", &num_ops);

	ll = LL_create();

	elapsed_time = 0;
	for(i = 1; i <= num_ops; ++i){
		fscanf(fp, "%d %d", &op, &val);
		gettimeofday(&ts_start, NULL);
		(op == 1) ? LL_insert(ll, val) : LL_delete(ll, val);
		gettimeofday(&ts_end, NULL);
		elapsed_time += (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		time_arr[i] += elapsed_time;
		space_arr[i] = SIZE;
	}

	fclose(fp);

	if(code == 2){
		gettimeofday(&ts_start, NULL);
		LL_traverse(ll, &num);
		gettimeofday(&ts_end, NULL);
		elapsed_time = (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		strcpy(access_output, "../output/im-linked-list-access.csv");
		if(access(access_output, F_OK) == 0){//file exists
			fp = fopen(access_output, "a");
		}else{
			fp = fopen(access_output, "w");
			fprintf(fp, "num,time_micro\n");
		}
		fprintf(fp, "%d,%lu\n", num, elapsed_time);
		fclose(fp);
	}

}

int get_num_ops(char *filename){

	FILE *fp;
	int num_ops;

	fp = fopen(filename, "r");
	if(fp == NULL){
		fprintf(stdout, "ERROR: %s does not exist\n", filename);
		fflush(stdout);
		exit(1);
	}
	fscanf(fp, "%d", &num_ops);
	fclose(fp);

	return num_ops;

}

void populate_output_filename(char *output, char *workload){

	int num;
	int i;
	int j;
	char num_str[256];

	memset(num_str, 0, 256);

	for(i = 0, j = 0; workload[i] != '\0'; ++i){
		if(workload[i] >= '0' && workload[i] <= '9'){
			num_str[j++] = workload[i];
		}
	}

	num = atoi(num_str);
	sprintf(output, "../output/im-linked-list-per-operation-%d.csv", num);

}

int main(int argc, char *argv[]){

	FILE *fp;
	int i;
	int code;
	char workload[256];
	char output[256];
	char WFile[256];
	unsigned long space_arr[1001];
	unsigned long time_arr[1001];
	unsigned long MBuffSize;
	int num_ops;

	if(argc != 3){
		fprintf(stdout, "USAGE: %s <workload-relative-to-cspot-directory> <code (1:update 2:access)>\n", argv[0]);
		exit(1);
	}

	strcpy(workload, argv[1]);
	populate_output_filename(output, workload);
	code = atoi(argv[2]);

	num_ops = get_num_ops(workload);
	memset(space_arr, 0, (num_ops + 1) * sizeof(unsigned long));
	memset(time_arr, 0, (num_ops + 1) * sizeof(unsigned long));

	strcpy(WFile, "storage");
	MBuffSize = 100 * num_ops * sizeof(NODE);
	PmallocInit(WFile, MBuffSize, 1);

	fprintf(stdout, "running workload: %s\n", workload);
	run_workload(workload, time_arr, space_arr, code);

	if(code == 1){
		fp = fopen(output, "w");
		fprintf(fp, "op,time_micro,space_bytes\n");
		for(i = 1; i <= num_ops; ++i){
			fprintf(fp, "%d,%.2lf,%.2lf\n", i, (double)time_arr[i], (double)space_arr[i]);
		}
		fclose(fp);
	}

	return 0;

}
