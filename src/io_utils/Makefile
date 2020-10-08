
CC = g++
CFLAGS = -Wall -g

lsh: lsh.o io_utils.o 
	$(CC) -o lsh lsh.o io_utils.o -DLSH $(CFLAGS)

cube: cube.o io_utils.o 
	$(CC) -o cube cube.o io_utils.o $(CFLAGS)

lsh.o: lsh.cpp 
	$(CC) -c lsh.cpp $(CFLAGS)

cube.o: cube.cpp
	$(CC) -c cube.cpp $(CFLAGS)

io_utils.o: io_utils.cpp
	$(CC) -c io_utils.cpp $(CFLAGS)

clean: 
	rm lsh lsh.o io_utils.o 
