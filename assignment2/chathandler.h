#ifndef DIST_H
#define DIST_H

#define TIMEOUT 30

/*
	Prepares the message by attaching length byte numchar to the front of the message
	at an offset index offs.
	Package the message into the buffer position "offs" of the buffer.
*/
void prepareMessage(unsigned char* sndbuf, int offs, unsigned char* message, unsigned int numchar) {
	sndbuf[offs] = (unsigned char)numchar;

	int i;
	for (i=1+offs; i<=numchar+offs; i++) {
		sndbuf[i]=message[i-1-offs];
	}
 }

/*
	Helper method to append message fragment to message package at an offs index.
	Frag info can consist of len byte in first index and username string
	returns total length of message
*/
int appendFrag(unsigned char* sndbuf, int offs, unsigned int len, unsigned char* username) {
	int i;
	for (i=0; i<=len; i++) {
		sndbuf[i+offs]=username[i];
	}
	return(len+offs);
}


/*
	Wrapper for receiving nbytes from socket.
	Returns -1 if error;
	Otherwise returns 0.
*/
int receiveMessage(int socket, unsigned char* rcvbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while (nbytes<numchar) {
		if ((nbytes+=recv(socket, rcvbuf+nbytes, sizeof(unsigned char), 0)) == -1) {
			return -1;
		}
	}
	rcvbuf[nbytes]='\0';
	return 0;
}

/*
	Wrapper for sending nbytes (plus length byte) to socket
	Returns -1 if error;
	Otherwise returns 0.
*/
int sendMessage(int socket, unsigned char* sndbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while(nbytes<numchar+1) {
		if ((nbytes += send(socket, sndbuf+nbytes, sizeof(unsigned char), 0)) ==-1) { //+1 to account for length byte
			return -1;
		}
	}
	return 0;
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
