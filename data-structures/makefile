CC=gcc

CFLAGS=-g

all: Accumulator.o DoublyLinkedList.o HashTable.o Stack.o Vector.o

Accumulator.o: Accumulator.c
	${CC} ${CFLAGS} -c -o $@ $<

DoublyLinkedList.o: DoublyLinkedList.c
	${CC} ${CFLAGS} -c -o $@ $<

HashTable.o: HashTable.c
	${CC} ${CFLAGS} -c -o $@ $<

Stack.o: Stack.c
	${CC} ${CFLAGS} -c -o $@ $<

Vector.o: Vector.c
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -f *.o
