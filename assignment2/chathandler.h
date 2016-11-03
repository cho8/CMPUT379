#ifndef DIST_H
#define DIST_H

/*
	Prepares the message by attaching length byte to the front of the message.
	Package the message into the buffer position "i" of the buffer.
*/
void prepareMessage(unsigned char* sndbuf, int i, char* message, unsigned int numchar) {
	sndbuf[0] = (unsigned char)numchar;

	int i;
	for (i=1; i<=numchar; i++) {
		sndbuf[i]=message[i-1];
	}
 }

/*
	Wrapper for receiving nbytes from socket
*/
void receiveMessage(int socket, unsigned char* rcvbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while (nbytes<numchar) {
		nbytes+=recv(socket, rcvbuf, sizeof(unsigned char)*numchar, 0);
		// printf(" rcvbuf: %s\n", rcvbuf);
	}
}

/*
	Wrapper for sending nbytes (plus length byte) to socket
*/
void sendMessage(int socket, unsigned char* sndbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while(nbytes<numchar) {
		nbytes += send(socket, sndbuf, sizeof(unsigned char)*(numchar+1), 0); //+1 to account for length byte
	}
}

/*
	Print n bytes (plus length byte) of content in a buffer.
	If 0 numchar, print the whole buf.
*/
void printBuf(unsigned char* buf, unsigned int numchar) {

		printf("[Buffer] : ");
	if (numchar ==0) {
		printf("%s\n", buf);
	} else {
		int i;
		for (i=0; i<=numchar; i++) {
			printf("%c", (unsigned char) buf[i]);
		} printf("\n");
	}
}

#endif
