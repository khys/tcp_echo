#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

// #define SRVPORT "49154"

struct msg_echo {
	unsigned short seq;
	unsigned short reserve;
	char msg[32];
};

int main(int argc, char *argv[])
{
    int sd, sd1, err, cnt;
    struct addrinfo hints, *res;
    struct sockaddr_storage sin;
    struct msg_echo echo;
    
    socklen_t sktlen, len;
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    if (argc != 2) {
		fprintf(stderr, "Usage: ./tcpsrv <port number>\n");
		exit(1);
	}
    
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(NULL, argv[1], &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }
    if ((sd = socket(res->ai_family, res->ai_socktype,
                     res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }
    if (bind(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }
    freeaddrinfo(res);
    if (listen(sd, 5) < 0) {
        perror("lisen");
        exit(1);
    }
    sktlen = sizeof (struct sockaddr_storage);
    if ((sd1 = accept(sd, (struct sockaddr *)&sin, &sktlen)) < 0) {
        perror("accept");
        exit(1);
    }
    if ((err = getnameinfo((struct sockaddr *)&sin, len, hbuf,
                           sizeof(hbuf), sbuf, sizeof(sbuf), 0)) < 0) {
        fprintf(stderr, "getnameinfo: %s\n", gai_strerror(err));
        exit(1);
    }
    
    for (;;) {
        if ((cnt = recv(sd1, &echo, sizeof echo, 0)) < 0) {
            perror("recv");
            exit(1);
        }
        echo.msg[cnt - sizeof(unsigned short) * 2] = '\0';
        printf("%d bytes recved: IP %s, port %s, seq %d, msg %s\n",
               cnt, hbuf, sbuf, echo.seq, echo.msg);

        echo.seq++;
        
        if ((cnt = send(sd1, &echo, sizeof echo, 0)) < 0) {
            perror("send");
            exit(1);
        }
        printf("%d bytes sent\n", cnt);
    }
    close(sd);
}
