all: libDummy.so prog 

CFLAGS= -g -Wall

libDummy.so: myLib.c
	gcc -o libDummy.so $(CFLAGS) -fpic myLib.c -shared

prog: prog.c
	gcc -o prog $(CFLAGS) prog.c