/*
 ============================================================================
 Name        : UDPServer.c
 Author      : W. Schilling
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : This file demonstrates a UDP receive operation.  It will receive a string from a client.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFSIZE (1024 + 4)

int main(int argc, char* argv[]) {
	// Verify that the command line arguments are correct.
	if (argc != 2) {
		printf("Usage: %s port\n", argv[0]);
		exit(0);
	}

	int port = atoi(argv[1]);

	struct sockaddr_in myaddr; /* our address */
	struct sockaddr_in remaddr; /* remote address */
	socklen_t addrlen = sizeof(remaddr); /* length of addresses */

	int retVal; /* # bytes received */
	int sockfd; /* our socket */
	unsigned char buf[BUFSIZE]; /* receive buffer */

	/* create a UDP socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		exit(-1);
	}

	/* bind the socket to the port that we wish to listen to. */
	memset((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);

	if (bind(sockfd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	do {

		/* now loop, receiving data and printing what we received */
		retVal = recvfrom(sockfd, &buf[0], BUFSIZE, 0, (struct sockaddr *) &remaddr, &addrlen);

		if (retVal > 0) {
			// Print out the length
			int lt = ntohl(*((int*)&buf[0]));
			char* msgToPrint = &buf[4];

			printf("Message of Length %d received: %s\n", lt, msgToPrint);
			if (strcmp(&buf[4], "QUIT") == 0) {
				retVal = -1;
			}
		}
	} while (retVal != -1);
}

