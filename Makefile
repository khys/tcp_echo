all: tcpsrv tcpcli

tcpsrv: tcpsrv.o
	gcc -o tcpsrv tcpsrv.o

tcpcli: tcpcli.o
	gcc -o tcpcli tcpcli.o

tcpsrv.o: tcpsrv.c
	gcc -c tcpsrv.c

tcpcli.o: tcpcli.c
	gcc -c tcpcli.c

clean:
	\rm tcpsrv tcpcli *.o
