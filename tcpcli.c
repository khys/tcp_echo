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

#define SRVPORT "49154"

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	char *node, buf[80];
	int sd, err, cnt;
	
	char serv[] = SRVPORT;
	node = argv[1];
	
	if (argc != 2) {
		fprintf(stderr, "Usage: ./tcpcli IP\n");
		exit(1);
	}	
	memset(&hints, 0, sizeof hints);
	hints.ai_socktype = SOCK_STREAM;
	if ((err = getaddrinfo(node, serv, &hints, &res)) < 0) {
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
		printf("message to be sent: ");
		if (fgets(buf, sizeof buf, stdin) == NULL) {
			break;
		}
		buf[strlen(buf) - 1] = '\0';

		if ((cnt = send(sd, buf, sizeof buf, 0)) < 0) {
			perror("send");
			exit(1);
		}
		printf("%d bytes sent\n", cnt);

		if ((cnt = recv(sd, buf, sizeof buf, 0)) < 0) {
			perror("recv");
			exit(1);
		}
		buf[cnt] = '\0';
		printf("%d bytes recved: IP %s, port %s, msg %s\n",
			   cnt, node, serv, buf);
	}
	printf("EOF received from keyboard\n");
	close(sd);
}
