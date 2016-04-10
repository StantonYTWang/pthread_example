#example of pthread
CC=g++

all:
	$(CC) pthread_example.cpp -o example -lrt

run:
	./example
 	
clean:
	rm example
