EXECUTABLE=p3
CFLAGS= -Wall -g

p3: p3.o auxFunc.o list.o
	gcc $(CFLAGS) -o $(EXECUTABLE) p3.o list.o auxFunc.o

p3.o: p3.c libraries.h
	gcc $(CFLAGS) -c p3.c

auxFunc.o: auxFunc.c libraries.h
	gcc $(CFLAGS) -c auxFunc.c

list.o: list.c list.h
	gcc $(CFLAGS) -c list.c

clc:
	rm *.o p3


