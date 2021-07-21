#ifndef REMOTE_SEQUENCER_H
#define REMOTE_SEQUENCER_H

#include "Sequencer.h"

#define REMSEQ_STR_SIZE 550

struct RemoteSequencer{
	SEQ seq;
	SEQCOMP seqcomp;
};

typedef struct RemoteSequencer REMSEQ;

char *REMSEQ_str(REMSEQ remseq);

#endif
