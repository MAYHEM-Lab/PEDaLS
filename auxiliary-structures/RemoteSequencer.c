#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RemoteSequencer.h"

char *REMSEQ_str(REMSEQ remseq){

	int retsize;
	char *seq_retval;
	char *retval;

	retsize = REMSEQ_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	seq_retval = SEQ_str(remseq.seq);
	sprintf(retval, "%s %lu ", seq_retval, remseq.seqcomp.seq_no);

	free(seq_retval);

	return retval;
}
