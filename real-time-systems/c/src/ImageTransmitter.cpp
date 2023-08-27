/**
 * @file ImageTransmitter.cpp
 * @author  Walter Schilling (schilling@msoe.edu)
 * @version 1.0
 *
 * @section LICENSE
 *
 *
 * This code is developed as part of the MSOE SE3910 Real Time Systems course,
 * but can be freely used by others.
 *
 * SE3910 Real Time Systems is a required course for students studying the
 * discipline of software engineering.
 *
 * This Software is provided under the License on an "AS IS" basis and
 * without warranties of any kind concerning the Software, including
 * without limitation merchantability, fitness for a particular purpose,
 * absence of defects or errors, accuracy, and non-infringement of
 * intellectual property rights other than copyright. This disclaimer
 * of warranty is an essential part of the License and a condition for
 * the grant of any rights to this Software.
 *
 * @section DESCRIPTION
 *      This class will transmit an image to a remote device.  The image will be transmitted as a set of UDP datagrams.
 */

#include "ImageTransmitter.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include "time_util.h"
#include <string.h>
#include <iostream>

/**
 * This will instantiate a new instance of this class. It will copy the machine name into a heap allocated string and update the port.
 * @param machineName This is the name of the machine that the image is to be streamed to.
 * @param port This is the udp port number that the machine is to connect to.
 * @param linesPerUDPDatagram This is the number of lines that are to be sent in each UDP datagram.
 */
ImageTransmitter::ImageTransmitter(char *machineName, int port,	int linesPerUDPDatagram) {
	destinationMachineName = machineName;
	myPort = port;
	this->linesPerUDPDatagram = linesPerUDPDatagram;
}

/**
 * This is the destructor. It will free all allocated memory.
 */
ImageTransmitter::~ImageTransmitter() {

}

/**
 * This method will stream via udp the image to the remote device.
 * @param image This is the image that is to be sent.
 * @return The return will be 0 if successful or -1 if there is a failure.
 */
int ImageTransmitter::streamImage(Mat* image) {
	/**
	 * 1.0 If the image and destination machine are not null,
	 */

	if ((image != NULL) && (destinationMachineName != NULL)) {
		struct sockaddr_in serv_addr;
		struct hostent *server;
		/**
		 * 1.1 Increment the image count.
		 */
		imageCount++;


		/**
		 * 1.2 Initialize the socket sockfd to be a DGRAM.
		 */
		if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("cannot create socket\n");
			return (-1);
		}


		/**
		 * 1.3 If there is an error with 1b, abort with an error message and return -1.
		 */
		if (sockfd < 0) {
			perror("ERROR opening socket");
			return -1;
		}

		/**
		 * 1.4 Get the host by name and set up a server instance.
		 */

		server = gethostbyname(destinationMachineName);

		/**
		 * 1.5 If the server is NULL, print out an error and return -1.
		 */

		if (server == NULL) {
			perror("ERROR, no such host\n");
			return -1;;
		}

		/**
		 * 1.6 Set up the rest of the UDP parameters.
		 * */
		// Zero out the structure.
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		// Now copy the appropriate data over.
		memcpy((char *) &serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);


		/**
		 * 1.7 Set the port for the system.
		 */
		serv_addr.sin_port = htons(myPort);


		/**
		 * 1.8 Obtain the image rows, columns, message size, and required buffer allocation size (which is ((3 * columns + 24) * linesPerUDPDatagram) + 4).
		 */
		int imageRows = image->size().height;
		int imageCols = image->size().width;
		int msgSize = ((3 * imageCols) + 24);
		int reqBufferAllocSize = ((msgSize) * linesPerUDPDatagram) + 4;

		/**
		 * 1.9 Allocate a buffer of message size length on the heap.
		 */
		void *msgToSend = malloc(reqBufferAllocSize);

		/**
		 * 1.10 Obtain the current timestamp in ms using the time_util library.
		 */

		int time = current_timestamp();

		/**
		 * 1.11 Declare a pointer to a 32 bit integer that has the same address as the start of the buffer.
		 * The, set the first 32 bits of the buffer to be the network copnverted endianess of the linesPerUDPDatagram variable.
		 */
		((uint32_t*) msgToSend)[0] = htonl(linesPerUDPDatagram);


		/**
		 * 1.12 Declare a variable that will keep track of the index into the image (i.e. which row is being packed right now),
		 * as well as another variable which counts how many rows have been packed into the given UDP datagram.
		 */
		int index = 0;
		int rows = imageRows;
		int rowsPacked = 0;

		int bytesPerImageCol = 3;



		/**
		 * 1.11 Iterate (using index) so longas the index is less than rows in the image.
		 */

		while (index < rows) {
			/**
			 * 1.11.1 Using a for loop, place up to linesPerUDPDatagram within the UDP message.  However, also make certain that we do not pack rows which do not exist into the UDP datagram.
			 * To do this, in addition to the udpLineNumber being less than linesPerUDPDatagram, the conditional on the for loop should also check to make sure that rowsPacked < rows (i.e. the number of rows that has been packed for transmission is less than the number of rows overall.
			 */
			for (int udpLineNumber = 0; (udpLineNumber < linesPerUDPDatagram) && (rowsPacked < rows); udpLineNumber++) {
				/**
				 * 1.11.1.1 Starting after the initial 4 bytes of the udp, which contains the linesPerUDPDatagram attribute, create the portion of the message which has the following:
				 * Integer 0: The start time for the transmission
				 * Integer 1: The current timestamp for the current portion of the image
				 * Integer 2: The count of the image.
				 * Integer 3: The number of rows in the image.
				 * Integer 4: The number of columns in the image
				 * Integer 5: The index being sent
				 * Followed by: An array of columns * 3 bytes, representing the pixels in the current row.
				 *
				 * The integers all need to have their endianess corrected before being sent.  The data array does not.
				 *
				 * Overall, there will be linesPerUDPDatagram instances of this in the allocated space, representing linesPerUDPDatagram lines being sent out at a time.
				 */
				//start time of the transmission
				((uint32_t*) msgToSend) [(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 1] = htonl(time);
				//current timestamp for current portion of the image
				uint32_t imageMessageTimestamp = current_timestamp();
				((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 2] = htonl(imageMessageTimestamp);
				//count of the image
				((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 3] = htonl(imageCount);
				//num rows
				((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 4] = htonl(imageRows);

				((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 5] = htonl(imageCols);

				((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 6] = htonl(rowsPacked);

				memcpy(&((uint32_t*) msgToSend)[(udpLineNumber * ((6 + (imageCols * bytesPerImageCol / 4)))) + 7], image->ptr(rowsPacked), imageCols * 3);
				/**
				 * 1.11.1.2 Increment the rows packed variable to indicate that another row has been packed.
				 */
				rowsPacked++;

			}
			/**
			 * 1.11.2 Send to message to the destination.
			 */
			// Note: The 1024 shouldn't be a magic number like this.  It is done like this to show you that the message length to send is the 1024 of the message plus the 4 bytes of the length up front.
			//DO WE NEED THE PLUS QUATRO

			int lres = sendto(sockfd, msgToSend, (reqBufferAllocSize + 4), 0, (struct sockaddr*) & serv_addr, sizeof(serv_addr));
			if (lres < 0) {
				printf("%d c %d ", lres, errno);
				perror("Transmit:");
				exit(-1);
			}


			/**
			 * 1.11.3 Increment the index to account the lines that were sent.
			 */
			index += linesPerUDPDatagram;

		}

		/**
		 * 1.12	Free the message that was allocated on the heap.
		 */
		free(msgToSend);

		/**
		 * 1.13 Close the socket down.
		 */
		close(sockfd);

	}
	return 0;
}
