#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "mio.h"
#include "pmalloc.h"

unsigned long SIZE;

struct Node{
	int val;
	struct Node *left;
	struct Node *right;
	struct Node *parent;
};

typedef struct Node NODE;

NODE *NODE_create(int val){
	
	NODE *node;

	//node = (NODE *)malloc(sizeof(NODE));
	node = (NODE *)Pmalloc(sizeof(NODE));
	node->val = val;
	node->left = NULL;
	node->right = NULL;
	node->parent = NULL;

	SIZE += sizeof(NODE);

	PmallocSyncObject((unsigned char *)node, sizeof(NODE));

	return node;

}

typedef struct BST{
	NODE *head;
} BST;

BST *BST_create(){
	
	BST *bst;

	//bst = (BST *)malloc(sizeof(BST));
	bst = (BST *)Pmalloc(sizeof(BST));
	bst->head = NULL;

	SIZE += sizeof(BST);

	PmallocSyncObject((unsigned char *)bst, sizeof(BST));

	return bst;

}

void BST_insert(BST *bst, int val){

	NODE *iterator;

	if(bst == NULL) return;

	if(bst->head == NULL){
		bst->head = NODE_create(val);
		return;
	}

	iterator = bst->head;
	while(1){
		if(val < iterator->val){
			if(iterator->left == NULL){
				iterator->left = NODE_create(val);
				iterator->left->parent = iterator;
				return;
			}else{
				iterator = iterator->left;
			}
		}else{
			if(iterator->right == NULL){
				iterator->right = NODE_create(val);
				iterator->right->parent = iterator;
				return;
			}else{
				iterator = iterator->right;
			}
		}
	}

}

NODE *search(BST *bst, int val){

	NODE *iterator;

	if(bst == NULL) return NULL;

	iterator = bst->head;
	while(1){
		if(iterator == NULL) return NULL;
		if(iterator->val == val) return iterator;
		
		if(val < iterator->val) iterator = iterator->left;
		else if(val > iterator->val) iterator = iterator->right;
	}

	return NULL;

}

/* gets called only when both children present */
NODE *get_predecessor(NODE *node){

	NODE *pred;

	if(node == NULL) return NULL;

	pred = node->left;

	while(pred->right != NULL){
		pred = pred->right;
	}

	return pred;

}

void BST_delete(BST *bst, int val){

	NODE *target;
	NODE *parent;
	NODE *pred;

	target = search(bst, val);
	if(target == NULL) return;
	parent = target->parent;

	SIZE -= sizeof(NODE);
	//TODO: deallocate

	if(target->left == NULL && target->right == NULL){//both children absent
		if(parent == NULL){//target is head
			bst->head = NULL;
			return;
		}
		if(parent->right == target){
			parent->right = NULL;
		}else{
			parent->left = NULL;
		}
	}else if(target->left != NULL && target->right == NULL){//only right child absent
		if(parent == NULL){
			bst->head = target->left;
			bst->head->parent = NULL;
			return;
		}
		if(parent->right == target){
			parent->right = target->left;
		}else{
			parent->left = target->left;
		}
		target->left->parent = parent;
	}else if(target->left == NULL && target->right != NULL){//only left child absent
		if(parent == NULL){
			bst->head = target->right;
			bst->head->parent = NULL;
			return;
		}
		if(parent->right == target){
			parent->right = target->right;
		}else{
			parent->left = target->right;
		}
		target->right->parent = parent;
	}else{//none of the children absent
		pred = get_predecessor(target);
		target->val = pred->val;
		parent = pred->parent;
		if(target->left == pred){//immediate left child is predecessor
			target->left = pred->left;
			if(target->left != NULL) target->left->parent = target;
		}else{
			parent->right = pred->left;
			if(pred->left != NULL) pred->left->parent = parent;
		}
	}

}

void preorder(NODE *node){
	if(node == NULL) return;
	fprintf(stdout, "%d ", node->val);
	preorder(node->left);
	preorder(node->right);
}

void print_preorder(BST *bst){
	preorder(bst->head);
	fprintf(stdout, "\n");
}

void BST_traverse_max(BST *bst, int *num){
	NODE *iterator;

	*num = 0;

	if(bst == NULL) return;
	iterator = bst->head;

	while(iterator != NULL){
		*num += 1;
		iterator = iterator->right;
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
	BST *bst;

	fp = fopen(workload, "r");
	fscanf(fp, "%d", &num_ops);

	bst = BST_create();

	elapsed_time = 0;
	for(i = 1; i <= num_ops; ++i){
		fscanf(fp, "%d %d", &op, &val);
		gettimeofday(&ts_start, NULL);
		(op == 1) ? BST_insert(bst, val) : BST_delete(bst, val);
		gettimeofday(&ts_end, NULL);
		elapsed_time += (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		time_arr[i] += elapsed_time;
		space_arr[i] = SIZE;
	}

	fclose(fp);

	if(code == 2){
		gettimeofday(&ts_start, NULL);
		BST_traverse_max(bst, &num);
		gettimeofday(&ts_end, NULL);
		elapsed_time = (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		strcpy(access_output, "../output/im-bst-access.csv");
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
	sprintf(output, "../output/im-bst-per-operation-%d.csv", num);

}

int main(int argc, char *argv[]){

	FILE *fp;
	int code;
	int i;
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
