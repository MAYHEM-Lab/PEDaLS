CC=gcc

MIOINC=../../mio

MIOOBJ=${MIOINC}/pmalloc.o ${MIOINC}/mio.o ${MIOINC}/mymalloc.o

CFLAGS=-g -I${MIOINC}

all: IMBSTLink.o IMBSTNode.o IMBSTStack.o IMBST.o app

IMBSTLink.o: IMBSTLink.c
	${CC} ${CFLAGS} -c -o $@ $<

IMBSTNode.o: IMBSTNode.c
	${CC} ${CFLAGS} -c -o $@ $<

IMBSTStack.o: IMBSTStack.c IMBSTLink.o IMBSTNode.o
	${CC} ${CFLAGS} -c -o $@ $<

IMBST.o: IMBST.c IMBSTLink.o IMBSTNode.o IMBSTStack.o
	${CC} ${CFLAGS} -c -o $@ $<

app: app.c IMBST.o IMBSTStack.o
	${CC} ${CFLAGS} -o $@ $< IMBSTLink.o IMBSTNode.o IMBST.o IMBSTStack.o ${MIOOBJ}

clean:
	rm -f *.o app storage
