CC = g++ -Wall -g	

all: main

main: helper.o main.o
	$(CC) -pthread -o main helper.o main.o

main.o: main.cc helper.h
	$(CC) -c main.cc

helper.o: helper.cc helper.h
	$(CC) -c helper.cc

tidy:
	rm -f *.o core

clean:
	rm -f main producer consumer *.o core
