/*
 Name        : UDPCLient.c
 Author      : W. Schilling
 Version     : 1.0
 Copyright   : Your copyright notice
 Description : This file demonstrates a UDP transmit operation.  It will transmit a string to the server.
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

/**
 * This method will send a message to the server.
 * @param msg This is a pointer to the message that is to be sent.
 * @param destinationMachineName This is the name of the machine that is to receive the message.
 * @param port This is the port that is to receive the item.
 */
static void sendMessage(char* msg, char* destinationMachineName, int port);

int main(int argc, char* argv[]) {
	// Verify that the command line arguments are correct.
	if (argc != 3) {
		printf("Usage: %s ipAddress port\n", argv[0]);
		exit(0);
	}

	// Allocate a buffer to use.
	char* msg = malloc(1024);

	do {
		// Obtain a message from the user.
		scanf("%1023s", msg);

		// Send it to the other end via a UDP socket.
		// The protocol is that the message will start with an integer length and then be followed by the string.  The size will be fixed.
		sendMessage(msg, argv[1], atoi(argv[2]));

		// So long as the message is not quit,
	} while (strcmp(msg, "QUIT") != 0);
	free(msg);
}

void sendMessage(char* msg, char* destinationMachineName, int port) {
	if (msg != NULL && destinationMachineName != NULL) {
		struct sockaddr_in serv_addr;
		struct hostent *server;
		int serverlen;

		// Create a socket for the data gram.
		int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

		// Make sure the socket is valid.
		if (sockfd < 0) {
			perror("ERROR opening socket");
			exit(-1);
		}

		// Obtain the server based upon the name passed in.
		server = gethostbyname(destinationMachineName);

		// Make sure the server was valid.
		if (server == NULL) {
			fprintf(stderr, "ERROR, no such host\n");
			exit(-1);
		}

		// Zero out the structure.
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		// Now copy the appropriate data over.
		memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

		serv_addr.sin_port = htons(port);

		// Now create the message to send.
		void *msgToSend = malloc(1024 + 4);

		// Add the string length up front.
		((int*) msgToSend)[0] = htonl(strlen(msg));

		// Now add the text.
		strcpy(&msgToSend[4], msg);

		serverlen = sizeof(serv_addr);

		// Note: The 1024 shouldn't be a magic number like this.  It is done like this to show you that the message length to send is the 1024 of the message plus the 4 bytes of the length up front.
		int lres = sendto(sockfd, msgToSend, (1024 + 4), 0, (struct sockaddr *) &serv_addr, serverlen);
		if (lres < 0) {
			printf("%d c %d ", lres, errno);
			perror("Transmit:");
			exit(-1);
		}
		// Free the message and close down the open socket.
		free(msgToSend);
		close(sockfd);
	}
}



