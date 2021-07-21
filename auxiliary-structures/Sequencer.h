#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "Client.h"
#include "DataItem.h"

#define SEQ_STR_SIZE 530

/**
 * op can be 0 (delete), 1 (insert)
 **/
struct Sequencer{
	int op;
	DI di;
	CLIENT c;
};

typedef struct Sequencer SEQ;

struct SequencerCompletion{
	unsigned long seq_no;
};

typedef struct SequencerCompletion SEQCOMP;

SEQ *SEQ_create(int op, DI di, CLIENT *c);
char *SEQ_str(SEQ seq);

#endif
