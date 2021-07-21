#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Sequencer.h"

SEQ *SEQ_create(int op, DI di, CLIENT *c){

	SEQ *seq;

	seq = (SEQ *)malloc(sizeof(SEQ));
	seq->op = op;
	seq->di = di;
	memcpy((void *)&seq->c, c, sizeof(CLIENT));

	return seq;
}

char *SEQ_str(SEQ seq){
	
	char *retval;
	char *di_retval;
	char *client_retval;
	int retsize;

	retsize = SEQ_STR_SIZE;
	retval = (char *)malloc(retsize * sizeof(char));

	di_retval = DI_str(seq.di);
	client_retval = CLIENT_str(seq.c);
	sprintf(retval, "op:%d %s %s", seq.op, di_retval, client_retval);

	free(di_retval);
	free(client_retval);

	return retval;

}
