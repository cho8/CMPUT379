#ifndef DIST_H
#define DIST_H

/*
	Prepares the message by attaching length byte numchar to the front of the message
	at an offset index offs.
	Package the message into the buffer position "offs" of the buffer.
*/
void prepareMessage(unsigned char* sndbuf, int offs, unsigned char* message, unsigned int numchar) {
	sndbuf[offs] = (unsigned char)numchar;

	int i;
	for (i=1+offs; i<=numchar+offs; i++) {
		sndbuf[i]=message[i-1];
	}
	printf("%x",sndbuf[0]);
	for(int j=1; j<numchar+2; j++) {
		printf("%c",sndbuf[j]);
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
void printBuf(char* label, int offs, unsigned char* buf, unsigned int numchar) {

	printf("[%s] : %d ",label, numchar);
	if (numchar==0) {
		printf("%s\n", buf);
	} else {
		// printf("%d", (unsigned int) buf[0]);
		int i=offs;
		while(i<(numchar+offs)) {
			// printf("%d ",i);
			printf("%c", (unsigned char) buf[i]);
			i++;
		} printf("\n");
	}
}

#endif
