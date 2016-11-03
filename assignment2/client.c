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

#define	 MY_PORT  21259

void sigchld_handler(int sig)
{
    pid_t p;
    int status;

    while ((p=waitpid(-1, &status, WNOHANG)) != -1)
    {
       /* Handle the death of pid p */
			 exit(0);
    }
}

void parseServerMessage(int s, unsigned char* rcvbuf) {
	int len=0;
	unsigned int nbytes;						//bytes read
	unsigned char username[20];
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
			receiveMessage(s,rcvbuf,1);
			len=(unsigned int)rcvbuf[0];
			receiveMessage(s,rcvbuf,len);
			exit(0);

	}


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


	struct	sockaddr_in	server;
	struct	hostent		*host;

	fd_set clientfds;

	host = gethostbyname ("localhost");
  // connect to something outside of localhost

	// handle child signal
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigchld_handler;
	sigaction(SIGCHLD, &sa, NULL);


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

	// bzero (&server, sizeof (server)); // set everything to zero byte-to-byte
	// bcopy (host->h_addr, & (server.sin_addr), host->h_length); // copy stuff form server

	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = host->h_addrtype;
	server.sin_port = htons (atoi(argv[2]));

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	FD_ZERO(&clientfds);
	FD_SET(s, &clientfds);
	FD_SET(0, &clientfds); //stdin
	fdmax = s;

	select(fdmax + 1, &clientfds, NULL, NULL, NULL);

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

	pid_t pid = fork();
	if (pid < 0) exit(1);
	if (pid==0) {
			// printf ("Forking for read channel...\n");
			// child process for reading
			while (1) {

				parseServerMessage(s,rcvbuf);
			}
			close(s);

	} else {

		// parent process for writing
		while (1) {

			unsigned char ubuf[BUFSIZE];

			fgets(buf,BUFSIZE-1,stdin);
			strncpy((char*)ubuf, buf, 512);	// signed to unsigned char*

			unsigned int len = strlen(buf)-1;
			if(strncmp((char*)ubuf, "exit",4)==0) {
				break;
			}

			prepareMessage(sndbuf, 0, ubuf, len);
			sendMessage(s,sndbuf,len);

		}
		// close the child reading process
		kill(pid, SIGTERM);
		close (s);
	}


}
