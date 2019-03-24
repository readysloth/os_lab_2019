gcc -o tcpserver tcpserver.c -I.
./tcpserver --bufsize 100 --serv_port 40006
#./tcpclient --bufsize 100 --addr 127.0.0.1 --message pls,work --serv_port 40006