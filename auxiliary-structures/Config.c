#include <stdio.h>

#include "Config.h"

void CONFIG_dump(CONFIG config){
	fprintf(stdout, "---------CONFIG: START DUMP---------\n");
	fprintf(stdout, "%30s: %c\n", "nodeID", config.nodeID);
	fprintf(stdout, "%30s: %d\n", "num_of_extra_links", config.num_of_extra_links);
	fprintf(stdout, "%30s: %d\n", "num_of_links", config.num_of_links);
	fprintf(stdout, "%30s: %lu\n", "data_woof_size", config.data_woof_size);
	fprintf(stdout, "%30s: %lu\n", "link_woof_size", config.link_woof_size);
	fprintf(stdout, "%30s: %lu\n", "ap_woof_size", config.ap_woof_size);
	fprintf(stdout, "%30s: %lu\n", "config_woof_size", config.config_woof_size);
	fprintf(stdout, "%30s: %lu\n", "checkpoint_woof_size", config.checkpoint_woof_size);
	fprintf(stdout, "%30s: %lu\n", "operation_woof_size", config.operation_woof_size);
	fprintf(stdout, "%30s: %lu\n", "sequencer_woof_size", config.sequencer_woof_size);
	fprintf(stdout, "----------CONFIG: END DUMP----------\n");
}
