CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
NAME=exeIMS
CC = g++
all: main_navestie

main_navestie: main.cpp
	$(CC) $(CFLAGS) -o $(NAME) main.cpp $(PFLAGS)
run:
	./$(NAME)
clean:
	rm $(NAME)
