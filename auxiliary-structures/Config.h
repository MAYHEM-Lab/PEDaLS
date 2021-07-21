#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#include "Options.h"

struct Config{
	char nodeID;
	int num_of_extra_links;
	int num_of_links;
	bool replicated;
	unsigned long data_woof_size;
	unsigned long link_woof_size;
	unsigned long ap_woof_size;
	unsigned long config_woof_size;
	unsigned long checkpoint_woof_size;
	unsigned long operation_woof_size;
	unsigned long sequencer_woof_size;
	unsigned long max_counter_woof_size;
};

typedef struct Config CONFIG;

void CONFIG_dump(CONFIG config);

#endif
