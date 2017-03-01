#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define SRVPORT 49152

main()
{
	int s, cnt;
	char lbuf[80], msg[80];
	struct sockaddr_in skt;
	struct in_addr ipaddr;

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	printf("server IP address: ");
	fgets(lbuf, sizeof lbuf, stdin);

	if (inet_aton(lbuf, &ipaddr) < 0) {
		perror("inet_aton");
		exit(1);
	}

	memset(&skt, 0, sizeof skt);
	skt.sin_family = AF_INET;
	skt.sin_port = htons(SRVPORT);
	skt.sin_addr.s_addr = htonl(ipaddr.s_addr);

	if (connect(s, (struct sockaddr *)&skt, sizeof skt) < 0) {
		perror("connect");
		exit(1);
	}

	for (;;) {
		printf("message to be sent: ");
		if (fgets(msg, sizeof msg, stdin) == NULL) {
			break;
		}
		msg[strlen(msg) - 1] = '\0';

		if ((cnt = send(s, msg, sizeof msg, 0)) < 0) {
			perror("send");
			exit(1);
		}
		printf("%d bytes sent\n", cnt);

		if ((cnt = recv(s, msg, sizeof msg, 0)) < 0) {
			perror("recv");
			exit(1);
		}
		msg[cnt] = '\0';
		printf("%d bytes recved: IP %s, port %d, msg %s\n",
			   cnt, inet_ntoa(skt.sin_addr),
			   ntohs(skt.sin_port), msg);
	}
	printf("EOF received from keyboard\n");
	close(s);
}
