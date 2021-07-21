CC=gcc

MIOINC=../../mio
MIOOBJ=${MIOINC}/pmalloc.o ${MIOINC}/mio.o ${MIOINC}/mymalloc.o

CFLAGS=-g -Wall -I${MIOINC}

all: app

app: app.c
	${CC} ${CFLAGS} -o $@ $^ ${MIOOBJ}

clean:
	rm -f app
