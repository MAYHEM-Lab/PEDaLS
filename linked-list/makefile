CC=gcc

ASINC=../auxiliary-structures
ASOBJ=${ASINC}/VersionStamp.o ${ASINC}/AccessPointer.o ${ASINC}/Node.o ${ASINC}/Link.o ${ASINC}/Data.o ${ASINC}/DataItem.o ${ASINC}/Checkpoint.o ${ASINC}/Operation.o ${ASINC}/Sequencer.o ${ASINC}/Config.o ${ASINC}/Topology.o ${ASINC}/Knowledge.o ${ASINC}/Client.o ${ASINC}/RemoteSequencer.o ${ASINC}/VersionStampIdxMap.o ${ASINC}/ProgressStatus.o

DSINC=../data-structures
DSOBJ=${DSINC}/Stack.o ${DSINC}/HashTable.o ${DSINC}/Accumulator.o

OINC=../options

HINC=../helpers
HOBJ=${HINC}/CSPOTHelpers.o ${HINC}/GenericHelpers.o ${HINC}/ReplicationHelpers.o

WOOFC=../../cspot
SHEP_SRC=${WOOFC}/woofc-shepherd.c
WINC=${WOOFC}/woofc.h ${WOOFC}/woofc-access.h ${WOOFC}/woofc-cache.h 
LINC=${WOOFC}/log.h ${WOOFC}/host.h ${WOOFC}/event.h
LOBJ=${WOOFC}/log.o ${WOOFC}/host.o ${WOOFC}/event.o
WOBJ=${WOOFC}/woofc.o ${WOOFC}/woofc-access.o ${WOOFC}/woofc-cache.o
WHOBJ=${WOOFC}/woofc-host.o
SLIB=${WOOFC}/lsema.o
LIBS=${WOOFC}/uriparser2/liburiparser2.a -lpthread -lm -lczmq

DEP=../..
MLIB=${DEP}/mio/mio.o ${DEP}/mio/mymalloc.o
ULIB=${DEP}/euca-cutils/libutils.a

CFLAGS=-g -Wall -I${ASINC} -I${DSINC} -I${OINC} -I${HINC} -I${WOOFC} -I${DEP}/mio -I${DEP}/euca-cutils

all: LinkedList.o app

LinkedList.o: LinkedList.c
	${CC} ${CFLAGS} -c -o $@ $<

app: app.c
	${CC} ${CFLAGS} -o $@ $^ ${HOBJ} ${ASOBJ} ${DSOBJ} ${WOBJ} ${WHOBJ} ${SLIB} ${LOBJ} ${MLIB} ${ULIB} ${LIBS} LinkedList.o
	mkdir -p cspot; cp $@ ./cspot; cp ${WOOFC}/woofc-container ./cspot; cp ${WOOFC}/woofc-namespace-platform ./cspot

clean:
	rm -rf *.o app
