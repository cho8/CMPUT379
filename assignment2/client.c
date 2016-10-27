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
#include <strings.h>

#define	 MY_PORT  2222


int main(int argc, char *argv[]) {
	int	s, number, haddr;
	int BUFSIZE = 512;
	char buf[BUFSIZE];
	char rcvbuf[BUFSIZE];

	struct	sockaddr_in	server;

	struct	hostent		*host;

	unsigned char handbuf[2] = {0};						// handshake buffer

	host = gethostbyname ("localhost");
  // connect to something outside of localhost
  //server.sin_addr.s_addr = inet_addr('ip_address');

	if (argc != 4) {
		printf("Usage: %s <hostname> <portnumber> <username>\n", argv[0]);
		return 1;
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

	bzero (&server, sizeof (server)); // set everything to zero byte-to-byte
	bcopy (host->h_addr, & (server.sin_addr), host->h_length); // copy stuff form server
	// if connecting outside of localhost, don't need the above two lines

	server.sin_family = host->h_addrtype;
	server.sin_port = htons (MY_PORT);

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	if(recv(s, handbuf, sizeof(handbuf), 0) < 0){
		perror ("Client: cannot recieve handshake");
	} else {
	printf("Received handshake\n");

	if(send(s, argv[3], sizeof(handbuf), 0) == -1){
			perror("Username send failure");
	}

	printf ("Forking for read channel...\n");
	pid_t pid = fork();
	if (pid < 0) exit(1);
	if (pid==0) {
			// child process for reading
			printf("Read..\n");
			while (1) {



				//printf("Enter value: ");
				//number=getchar();
				//write(s,&number, sizeof(number));
				read (s, rcvbuf, BUFSIZE);
				if (strlen(rcvbuf)>1) {
					printf("%s", rcvbuf);
				}

				// fprintf (stderr, "Process %d gets number %d\n", getpid (),
				// 	ntohl (number));
				// sleep (5);
			}
			close(s);
	} else {

		while (1) {

			fgets(buf,BUFSIZE-1,stdin);
			write(s,buf, BUFSIZE);
		// read (s, &number, sizeof (number));

		// fprintf (stderr, "Process %d gets number %d\n", getpid (),
		// 	ntohl (number));
		// sleep (5);
		}
		close (s);
	}

	}
}
