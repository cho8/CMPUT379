/*
  TODO:
    X from commandline: chat379 <hostname> <portnumber> <username>
    X handshake (connection -> receives hex -> responds username)
		- clean up zombie children
    - chat UI (chat, user status)
    X chat message -- commandline input, then to server
    - keepalive
    - receive server errors
*/


#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "chathandler.h"
#include <signal.h>

#define STDIN 0


int parseServerMessage(int s, unsigned char* rcvbuf) {
	int len=0;
	unsigned int nbytes;						//bytes read
	unsigned char username[20];

	int exitcode=0;
	nbytes = receiveMessage(s,rcvbuf,1);	// get the code

	printf("%x ", rcvbuf[0]);
	switch (rcvbuf[0]) {
		case (unsigned char)0x00 :	// we a chat message

			nbytes = receiveMessage(s,rcvbuf,1);	// get the user length byte
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len); // get the user name bytes
			// printBuf("User",0,rcvbuf,len);
			printf("%s : ",rcvbuf);
			receiveMessage(s,rcvbuf,1);	// get the message length byte
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len+1); // get the message name bytes
			// printBuf("Message", 0, rcvbuf,len);
			printf("%s\n", rcvbuf);

			break;
		case 0x01 : // yo sum1 joined
			printf("\n=== User connected: ");
			receiveMessage(s,rcvbuf,1);
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len);
			printBuf("Connected", 0,rcvbuf,len); // accounts for the code and len
			break;
		case 0x02 :	// sum1 left
			printf("\n=== User disconnected: ");
			receiveMessage(s,rcvbuf,1);
			len=(unsigned int)rcvbuf[0];
			receiveMessage(s,rcvbuf,len);

			printBuf("Disconnected", 0,rcvbuf,len); // acounts for the code and len bytes
			break;
		case 0x11 : // okbai
			printf("\n=== Timed out\n");
			exitcode=-1;
			break;
	}

	return exitcode;
 }

int main(int argc, char *argv[]) {
	int	s;											//sock
	int e;											//exit
	int fdmax;									//max file descriptors
	unsigned int n_users; 			//number of users
	unsigned int nbytes;				//num bytes sent/received

	int BUFSIZE = 512;
	char buf[BUFSIZE];												// buffer for getting input
	unsigned char sndbuf[BUFSIZE];
	unsigned char rcvbuf[BUFSIZE];


	struct	sockaddr_in	server;
	struct	hostent		*host;

	fd_set clientfds;
	fd_set readfds;

	host = gethostbyname ("localhost");
  // connect to something outside of localhost


	if (argc != 4) {
		printf("Usage: %s <hostname> <portnumber> <username>\n", argv[0]);
		exit (0);
	}

	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}

	s = socket (AF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		perror ("Client: cannot open socket");
		exit (1);
	}


	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = host->h_addrtype;
	server.sin_port = htons (atoi(argv[2]));

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	FD_ZERO(&clientfds);
	FD_ZERO(&readfds);
	FD_SET(s, &clientfds);
	// FD_SET(STDIN, &clientfds); //stdin
	fdmax = s;

	select(fdmax + 1, &clientfds, NULL, NULL, 0);

	// Client expects server socket to be ready for read past select
	if (! FD_ISSET(s, &clientfds)) {
		perror("handshake receive"); // error
		exit(1);
	}

	// ===== Get handshake bytes
	receiveMessage(s, rcvbuf, 2);

	if(rcvbuf[0] != (unsigned char)0xCF && rcvbuf[1] != (unsigned char)0xA7) {
		perror ("Client: failed recieve handshake");
		exit(1);
	}
	printf("Received handshake\n");

	// ==== Get number of users
	receiveMessage(s, rcvbuf, 1);
	n_users=(unsigned int)rcvbuf[0];

	printf("%d users connected.\n", n_users);
	if (n_users>0) {
		printf("Receiving userlist\n");
		for (int i=0; i<n_users; i++) {
			receiveMessage(s, rcvbuf, 1);
			unsigned int len = (unsigned int)rcvbuf[0];
			receiveMessage(s, rcvbuf, len);
			printf("-- %s\n", rcvbuf);
		}
	}

	//TODO ==== Send user name =====
	unsigned int userlen = strlen(argv[3]);
	sndbuf[0] = (unsigned char)userlen;
	int i=0;
	for (i=1; i<=userlen; i++) {
		sndbuf[i]=argv[3][i-1];
	}
	printBuf("send username", 1, sndbuf,userlen);
	sendMessage(s,sndbuf,userlen);

	printf("Joining the chat channel...\n");

	// start loops


	// parent process for writing
	while (1) {

		FD_CLR(s, &readfds);
    FD_SET(s, &readfds);
    FD_SET(STDIN, &readfds);
    select(fdmax+1, &readfds, NULL, NULL, NULL);

		if (FD_ISSET(STDIN, &readfds)) {
		// there's some keyboard input


				unsigned char ubuf[BUFSIZE];		// buff for getting around char* in fgets

				fgets(buf,BUFSIZE-1,stdin);
				strncpy((char*)ubuf, buf, 512);	// signed to unsigned char*

				unsigned int len = strlen(buf)-1;
				if(strncmp((char*)ubuf, "exit",4)==0 || e==-1) {
					break;
				}

				prepareMessage(sndbuf, 0, ubuf, len);
				sendMessage(s,sndbuf,len);

		}
		if (FD_ISSET(s, &readfds)) {
			// otherwise we got something from server
			if ((e=parseServerMessage(s,rcvbuf))==-1) {
				break;
			}

		}

	}

	close(s);
	close(STDIN);

}
