CC=gcc
CFLAGS=-I.
CURRENT_DIR=$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))


libFact.so : libFact.h 
	$(CC) -c -fPIC libFact.c -o libFact.o
	$(CC) -o libFact.so -shared libFact.o

server: libFact.so
	$(CC) -c server.c -o server.o
	$(CC) -o server server.o -L$(shell pwd) -lFact -pthread -Wl,-rpath=$(shell pwd) $(CFLAGS)

client: libFact.so
	$(CC) -c client.c -o client.o
	$(CC) -o client client.o -L$(shell pwd) -lFact -pthread -Wl,-rpath=$(shell pwd) $(CFLAGS)

all: libFact client server

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
clean_o:
	$(shell rm ./*.o)