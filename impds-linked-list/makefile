CC=gcc

MIOINC=../../mio

MIOOBJ=${MIOINC}/mio.o ${MIOINC}/pmalloc.o ${MIOINC}/mymalloc.o

CFLAGS=-g -I${MIOINC}

all: IMLLNode.o IMLLLink.o IMLL.o app

IMLLNode.o: IMLLNode.c
	${CC} ${CFLAGS} -c -o $@ $<

IMLLLink.o: IMLLLink.c
	${CC} ${CFLAGS} -c -o $@ $<

IMLL.o: IMLL.c
	${CC} ${CFLAGS} -c -o $@ $<

app: app.c
	${CC} ${CFLAGS} -o $@ $< IMLLNode.o IMLLLink.o IMLL.o ${MIOOBJ}

clean:
	rm -rf *.o app storage
