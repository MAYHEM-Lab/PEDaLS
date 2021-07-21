#ifndef VERSION_STAMP_IDX_MAP
#define VERSION_STAMP_IDX_MAP

#include "VersionStamp.h"

#define VSIDX_STR_SIZE 60

struct VersionStampIdxMap{
	VS vs;
	unsigned long idx;
};

typedef struct VersionStampIdxMap VSIDX;

char *VSIDX_str(VSIDX vsidx);

#endif
