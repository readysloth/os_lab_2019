CC=gcc
CFLAGS=-I.

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)
	
process_memory: process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)


all: parallel_min_max process_memory

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
clean_o:
	rm ./*.o
	
	
	
threadSum.so: threadSum.h
	$(CC) -c -fPIC threadSum.c -o threadSum.o 
	$(CC) -shared threadSum.o -o libthreadSum.so

	  
parallel_sum: threadSum.h threadSum.c utils.o threadSum.so 
	$(CC) -o psum.o -c parallel_sum.c
	$(CC) -o parallel_sum psum.o utils.o -L$(pwd) -lthreadSum -pthread $(CFLAGS)
	clean_o