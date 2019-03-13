mkCC=gcc
CFLAGS=-I.
CURRENT_DIR=$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))


udpclient :
	$(CC) -o udpclient udpclient.c $(CFLAGS)

tcpclient : 
	$(CC) -o tcpclient tcpclient.c $(CFLAGS)
	
udpserver :
	$(CC) -o udpclient udpclient.c $(CFLAGS)

tcpserver : 
	$(CC) -o tcpserver tcpserver.c $(CFLAGS)


all: tcpserver tcpclient udpserver udpclient

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
clean_o:
	$(shell rm ./*.o)