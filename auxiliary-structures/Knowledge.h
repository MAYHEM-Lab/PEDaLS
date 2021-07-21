#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include "VersionStamp.h"

#define KNOW_STR_SIZE 60

struct Knowledge{
	VS vs;
	unsigned long seq_no;
};

typedef struct Knowledge KNOW;

char *KNOW_str(KNOW k);
char *KNOW_get_k_woof_name_from_VS(VS vs);

#endif
