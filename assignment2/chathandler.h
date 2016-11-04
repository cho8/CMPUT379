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
	Wrapper for receiving nbytes from socket
*/
unsigned int receiveMessage(int socket, unsigned char* rcvbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while (nbytes<numchar) {
		nbytes+=recv(socket, rcvbuf+nbytes, sizeof(unsigned char)*numchar, 0);
	}
	rcvbuf[nbytes]='\0';
	return nbytes;
}

/*
	Wrapper for sending nbytes (plus length byte) to socket
*/
unsigned int sendMessage(int socket, unsigned char* sndbuf, unsigned int numchar) {
	unsigned int nbytes = 0;
	while(nbytes<numchar) {
		nbytes += send(socket, sndbuf+nbytes, sizeof(unsigned char)*(numchar+1), 0); //+1 to account for length byte
	}
	return nbytes;
}


/* Print chat */
void printChat(unsigned char* username, unsigned int userlen, unsigned char* buf, unsigned int numchar) {
	int i;
	for(i=0; i<userlen; i++){
		// printf("%d ",i);
		printf("%c", (unsigned char) username[i]);
		i++;
	} printf("\n");
	printf(" : ");
	for(i=0; i<numchar; ) {
		// printf("%d ",i);
		printf("%c", (unsigned char) buf[i]);
		i++;
	} printf("\n");
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
