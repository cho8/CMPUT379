/*
  TODO:
    X from commandline: chat379 <hostname> <portnumber> <username>
    X handshake (connection -> receives hex -> responds username)
    X chat UI (chat, user status)
    X chat message -- commandline input, then to server
    X keepalive
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
#include <sys/timeb.h>

#include "chathandler.h"

#define STDIN 0

int parseServerMessage(int s, unsigned char* rcvbuf) {
	int len=0;
	unsigned int nbytes;						//bytes read

	int exitcode=0;
	nbytes = receiveMessage(s,rcvbuf,1);	// get the code

	switch (rcvbuf[0]) {
		case (unsigned char)0x00 :	// we a chat message

			nbytes = receiveMessage(s,rcvbuf,1);	// get the user length byte
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len); // get the user name bytes
			printf("%s : ",rcvbuf);
			receiveMessage(s,rcvbuf,1);	// get the message length byte
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len+1); // get the message name bytes
			printf("%s\n", rcvbuf);
			break;

		case 0x01 : // yo sum1 joined
			printf("\n=== User connected: ");
			receiveMessage(s,rcvbuf,1);
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len);
			printf("%s ===\n",rcvbuf); // accounts for the code and len
			break;

		case 0x02 :	// sum1 left
			printf("\n=== User disconnected: ");
			receiveMessage(s,rcvbuf,1);
			len=(unsigned int)rcvbuf[0];

			receiveMessage(s,rcvbuf,len);
			printf("%s ===\n",rcvbuf); // accounts for the code and len
			break;

	}
	return exitcode;
 }

int main(int argc, char *argv[]) {
	int	s;											//sock
	int fdmax;									//max file descriptors
	unsigned int n_users; 			//number of users
	unsigned int nbytes;				//num bytes sent/received

	int BUFSIZE = 512;
	char buf[BUFSIZE];												// buffer for getting input
	unsigned char sndbuf[BUFSIZE];
	unsigned char rcvbuf[BUFSIZE];

	struct timeval	tv = {TIMEOUT,0};
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
		perror("handshake initiate"); // error
		exit(1);
	}

	// ===== Get handshake bytes
	if (receiveMessage(s, rcvbuf, 2)==-1) {
		perror("handshake receive");
	}

	if(rcvbuf[0] != (unsigned char)0xCF && rcvbuf[1] != (unsigned char)0xA7) {
		perror ("Client: failed recieve handshake");
		exit(1);
	}
	printf("=== Connecting to server... \n");

	// ==== Get number of users
	if (receiveMessage(s, rcvbuf, 1)==-1) {
		perror("number of users");
	}
	n_users=(unsigned int)rcvbuf[0];

	printf("=== %d users connected.\n", n_users);
	if (n_users>0) {
		for (int i=0; i<n_users; i++) {
			if (receiveMessage(s, rcvbuf, 1) ==-1) {
				perror(" get username len");
			}
			unsigned int len = (unsigned int)rcvbuf[0];
			if (receiveMessage(s, rcvbuf, len) == -1) {
				perror("get username");
			}
			printf("-- %s\n", rcvbuf);
		}
	}

	// ==== Send user name =====
	unsigned int userlen = strlen(argv[3]);
	sndbuf[0] = (unsigned char)userlen;
	int i=0;
	for (i=1; i<=userlen; i++) {
		sndbuf[i]=argv[3][i-1];
	}
	if (sendMessage(s,sndbuf,userlen) ==-1) {
		perror("send username");
	}

	printf("\nJoined the chat channel as [%s]...\n",argv[3]);

	// start loop
	while (1) {

		FD_CLR(s, &readfds);
    FD_SET(s, &readfds);
    FD_SET(STDIN, &readfds);
    if (select(fdmax+1, &readfds, NULL, NULL, &tv)==0) {

			// client has been idle. Send a keepalive.
			sndbuf[0]=0x00;
			sndbuf[1]=0;
			if (sendMessage(s,sndbuf,1)==-1) {
				perror("send keepalive");
			}
			struct timeval tv={.tv_sec=TIMEOUT,.tv_usec=0};
			continue;
		}

		if (FD_ISSET(STDIN, &readfds)) {
			// there's some keyboard input
			struct timeval tv={.tv_sec=TIMEOUT,.tv_usec=0};
			unsigned char ubuf[BUFSIZE];		// buff for getting around char* in fgets


			fgets(buf,BUFSIZE-1,stdin);			// fgets will block
			strncpy((char*)ubuf, buf, BUFSIZE);	// signed to unsigned char*

			unsigned int len = strlen(buf)-1;
			if(strncmp((char*)ubuf, "!exit",5)==0) {
				break;
			}

			if(strncmp((char*)ubuf, "\n",1)==0) {	// eat up return presses
				continue;
			}

			sndbuf[0]=(unsigned char)len;
			appendFrag(sndbuf,1,len,ubuf);
			if (sendMessage(s,sndbuf,len)==-1) {
				perror("send msg");
			}

		}
		if (FD_ISSET(s, &readfds)) {
			// otherwise we got something from server
			if ((parseServerMessage(s,rcvbuf))==-1) {
				break;
			}
		}

	}
	close(s);
	close(STDIN);

}
