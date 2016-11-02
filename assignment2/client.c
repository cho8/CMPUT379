/*
  TODO:
    - from commandline: chat379 <hostname> <portnumber> <username>
    - handshake (connection -> receives hex -> responds username)
		- clean up zombie children
    - chat UI (chat, user status)
    - chat message -- commandline input, then to server
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

#define	 MY_PORT  2222
#define  hostaddr "127.0.0.1"

unsigned char CODE_MSG = 0X00;
#define  CODE_ENT 0x01
#define  CODE_EXT 0x02

void packageMessage(int s, unsigned char* sndbuf, char* message, int numchar) {
	sndbuf[0] = CODE_MSG;
	send(s, sndbuf, sizeof(unsigned char),0);

	for (int i=0; i<numchar; i++) {
		send(s,message,sizeof(unsigned char),0);
	}
 }

void parseMessage(char* rcvbuf, char* buf) {
	// parse user-update status nessages sent by server
}

int main(int argc, char *argv[]) {
	int	s;																		//sock
	int fdmax;																//max file descriptors
	int n_users;															//number of users

	int BUFSIZE = 512;
	char buf[BUFSIZE];
	unsigned char sndbuf[BUFSIZE];
	char rcvbuf[BUFSIZE];
	unsigned char handbuf[2] = {0};						// handshake buffer

	struct	sockaddr_in	server;
	struct	hostent		*host;

	fd_set clientfds;

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

	// bzero (&server, sizeof (server)); // set everything to zero byte-to-byte
	// bcopy (host->h_addr, & (server.sin_addr), host->h_length); // copy stuff form server

	// server.sin_addr.s_addr = inet_addr("localhost");
	server.sin_family = host->h_addrtype;
	server.sin_port = htons (MY_PORT);

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	FD_ZERO(&clientfds);
	FD_SET(s, &clientfds);
	FD_SET(0, &clientfds); //stdin
	fdmax = s;

	select(fdmax + 1, &clientfds, NULL, NULL, NULL);

	//Client expects server socket to be ready for read past select
	if (! FD_ISSET(s, &clientfds)) {
		perror("handshake receive"); // error
		exit(1);
	}


	recv(s, handbuf, sizeof(handbuf), 0);
	if(handbuf[0] != (unsigned char)0xCF && handbuf[1] != (unsigned char)0xA7) {
		perror ("Client: failed recieve handshake");
		exit(1);
	}
	printf("Received handshake\n");

	recv(s, rcvbuf, sizeof(rcvbuf), 0);
	n_users=atoi(rcvbuf);

	printf("%d users connected.\n", n_users);
	// for (int i=0; i<atoi(rcvbuf); i++) {
	// 	// recv(s,rcvbuf, )
	// 	// printf()
	// }

	//TODO send username
	// if(send(s, argv[3], sizeof(handbuf), 0) == -1){
	// 		perror("Username send failure");
	// }

	printf ("Forking for read channel...\n");
	pid_t pid = fork();
	if (pid < 0) exit(1);
	if (pid==0) {
			// child process for reading
			while (1) {

				recv(s, rcvbuf, sizeof(rcvbuf), 0);
				printf("%s", rcvbuf);

			}
			close(s);
	} else {
		// parent process for writing
		while (1) {

			fgets(buf,BUFSIZE-1,stdin);
			int len = strlen(buf);
			if(strncmp(buf, "exit",4)==0) {
				break;
			}

			packageMessage(s, sndbuf, buf, len);
			// printf("Buf: %s\n", sndbuf);



		}
		// close the child reading process
		kill(pid, SIGTERM);
		close (s);
	}


}
