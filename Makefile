CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
 
CC = g++
all: main_navestie

main_navestie: main.cpp
	$(CC) $(CFLAGS) -o $@ main.cpp $(PFLAGS)
