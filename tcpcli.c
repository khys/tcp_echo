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
    int sd, err, cnt;
    struct addrinfo hints, *res;
    struct msg_echo echo;
    
    if (argc != 3) {
        fprintf(stderr, "Usage: ./tcpcli <IP address> <port number>\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(argv[1], argv[2], &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }
    if ((sd = socket(res->ai_family, res->ai_socktype,
                     res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }   
    if (connect(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(1);
    }
    freeaddrinfo(res);

    for (;;) {
        printf("message to be sent (FIN to exit): ");
        if (fgets(echo.msg, sizeof echo.msg, stdin) == NULL) {
            break;
        }
        echo.msg[strlen(echo.msg) - 1] = '\0';

        if (!strcmp(echo.msg, "FIN")) {
            printf("FIN received from client\n");
            break;
        } else if (echo.seq == 10) {
            printf("Finished: seq = 10\n");
            break;
        }
        
        if ((cnt = send(sd, &echo, sizeof echo, 0)) < 0) {
            perror("send");
            exit(1);
        }
        printf("%d bytes sent\n", cnt);

        if ((cnt = recv(sd, &echo, sizeof echo, 0)) < 0) {
            perror("recv");
            exit(1);
        }
        echo.msg[cnt - sizeof(unsigned short) * 2] = '\0';
        printf("%d bytes recved: IP %s, port %s, seq %d, msg %s\n",
               cnt, argv[1], argv[2], echo.seq, echo.msg);
    }
    close(sd);
}
