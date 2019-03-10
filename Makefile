CC = gcc
LUCC = bin/lucc
LUOP = bin/luop
LUGEN = bin/lugen
TESTDIR = test

all:
	make -C parser
	make -C optimizer
	make -C generator

test: all 
	./test.sh

clean:
	make -C parser clean
	make -C optimizer clean
	make -C generator clean

.PHONY: all test clean


