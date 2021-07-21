CC=gcc

DSINC=../data-structures
ASINC=../auxiliary-structures
OINC=../options
CSPOTPARENT=../..
WOOFCINC=${CSPOTPARENT}/cspot
MIOINC=${CSPOTPARENT}/mio
EUCAINC=${CSPOTPARENT}/euca-cutils
CSPOTINC=${CSPOTPARENT}/cspot

CFLAGS=-g -I${DSINC} -I${ASINC} -I${OINC} -I${WOOFCINC} -I${MIOINC} -I${CSPOTINC} -I${EUCAINC}

all: GenericHelpers.o CSPOTHelpers.o ReplicationHelpers.o

GenericHelpers.o: GenericHelpers.c
	${CC} ${CFLAGS} -c -o $@ $<

CSPOTHelpers.o: CSPOTHelpers.c
	${CC} ${CFLAGS} -c -o $@ $<

ReplicationHelpers.o: ReplicationHelpers.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -f *.o
