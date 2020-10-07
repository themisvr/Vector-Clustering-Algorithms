
CC = g++
CFLAGS = -Wall -g

all: test

test: test_main.o io_utils.o cmd_args.o
	$(CC) -o test test_main.o io_utils.o cmd_args.o $(CFLAGS)

test_main.o: test_main.cpp 
	$(CC) -c test_main.cpp $(CFLAGS)

io_utils.o: io_utils.cpp
	$(CC) -c io_utils.cpp $(CFLAGS)

cmd_args.o: cmd_args.cpp
	$(CC) -c cmd_args.cpp $(CFLAGS)

clean: 
	rm test test_main.o io_utils.o cmd_args.o 
