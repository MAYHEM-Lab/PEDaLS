CC=gcc

OINC=../options

CFLAGS=-g -I${OINC}

all: VersionStamp.o Node.o AccessPointer.o DataItem.o Data.o Link.o VersionStampIdxMap.o Checkpoint.o Config.o Client.o Knowledge.o Operation.o ProgressStatus.o Sequencer.o RemoteSequencer.o Topology.o TopologyNode.o

VersionStamp.o: VersionStamp.c
	${CC} ${CFLAGS} -c -o $@ $<

Node.o: Node.c
	${CC} ${CFLAGS} -c -o $@ $<

AccessPointer.o: AccessPointer.c
	${CC} ${CFLAGS} -c -o $@ $<

DataItem.o: DataItem.c
	${CC} ${CFLAGS} -c -o $@ $<

Data.o: Data.c
	${CC} ${CFLAGS} -c -o $@ $<

Link.o: Link.c
	${CC} ${CFLAGS} -c -o $@ $<

VersionStampIdxMap.o: VersionStampIdxMap.c
	${CC} ${CFLAGS} -c -o $@ $<

Checkpoint.o: Checkpoint.c
	${CC} ${CFLAGS} -c -o $@ $<

Client.o: Client.c
	${CC} ${CFLAGS} -c -o $@ $<

Config.o: Config.c
	${CC} ${CFLAGS} -c -o $@ $<

Knowledge.o: Knowledge.c
	${CC} ${CFLAGS} -c -o $@ $<

Operation.o: Operation.c
	${CC} ${CFLAGS} -c -o $@ $<

ProgressStatus.o: ProgressStatus.c
	${CC} ${CFLAGS} -c -o $@ $<

RemoteSequencer.o: RemoteSequencer.c
	${CC} ${CFLAGS} -c -o $@ $<

Sequencer.o: Sequencer.c
	${CC} ${CFLAGS} -c -o $@ $<

Topology.o: Topology.c
	${CC} ${CFLAGS} -c -o $@ $<

TopologyNode.o: TopologyNode.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -f *.o
