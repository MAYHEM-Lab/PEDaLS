#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "IMBST.h"
#include "IMBSTOptions.h"
#include "mio.h"
#include "pmalloc.h"

unsigned long SPACE = 0;

void run_workload(char *workload, int num_of_extra_links, unsigned long *time_arr, unsigned long *space_arr, int code){

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
	IMBST *bst;

	fp = fopen(workload, "r");
	fscanf(fp, "%d", &num_ops);

	bst = IMBST_init(num_of_extra_links, 1);

	elapsed_time = 0;
	for(i = 1; i <= num_ops; ++i){
		fscanf(fp, "%d %d", &op, &val);
		gettimeofday(&ts_start, NULL);
		(op == 1) ? IMBST_insert(bst, val) : IMBST_delete(bst, val);
		gettimeofday(&ts_end, NULL);
		elapsed_time += (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		time_arr[i] += elapsed_time;
		space_arr[i] = SPACE;
	}

	fclose(fp);

	if(code == 2){
		gettimeofday(&ts_start, NULL);
		IMBST_traverse_max(bst, &num);
		gettimeofday(&ts_end, NULL);
		elapsed_time = (ts_end.tv_sec * 1000000 + ts_end.tv_usec) - (ts_start.tv_sec * 1000000 + ts_start.tv_usec);
		strcpy(access_output, "../output/impds-bst-access.csv");
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

void populate_output_filename(char *output, char *workload, int num_of_extra_links){

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
	sprintf(output, "../output/impds-bst-per-operation-%d-%d.csv", num, num_of_extra_links);

}

int main(int argc, char *argv[]){

	FILE *fp;
	int i;
	char workload[256];
	char output[256];
	char WFile[256];
	unsigned long space_arr[1001];
	unsigned long time_arr[1001];
	unsigned long MBuffSize;
	int code;
	int num_ops;
	int num_of_extra_links;

	if(argc != 4){
		fprintf(stdout, "USAGE: %s <workload-relative-to-cspot-directory> <num-of-extra-links> <code (1:update 2:access)>\n", argv[0]);
		exit(1);
	}

	strcpy(workload, argv[1]);
	num_of_extra_links = atoi(argv[2]);
	populate_output_filename(output, workload, num_of_extra_links);
	code = atoi(argv[3]);

	num_ops = get_num_ops(workload);
	memset(space_arr, 0, (num_ops + 1) * sizeof(unsigned long));
	memset(time_arr, 0, (num_ops + 1) * sizeof(unsigned long));

	strcpy(WFile, "storage");
	MBuffSize = 100 * num_ops * sizeof(IMBSTNODE) * (2 + num_of_extra_links) * sizeof(IMBSTLINK);
	PmallocInit(WFile, MBuffSize, 1);

	fprintf(stdout, "running workload: %s\n", workload);
	run_workload(workload, num_of_extra_links, time_arr, space_arr, code);

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
