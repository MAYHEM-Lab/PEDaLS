#ifndef DATA_H
#define DATA_H

#define DATA_STR_SIZE 300

#include "DataItem.h"
#include "Options.h"
#include "VersionStamp.h"

struct Data{

    VS vs;
    DI di;
    char lw_name[MAX_SIZE_WOOF_NAME];

};

typedef struct Data DATA;

char *DATA_str(DATA data);

#endif
