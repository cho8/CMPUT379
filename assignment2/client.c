/*
  TODO:
    - from commandline: chat379 <hostname> <portnumber> <username>
    - handshake (connection -> receives hex -> responds username)
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


int main()
{
	int	s, number, haddr;

	struct	sockaddr_in	server;

	struct	hostent		*host;
  
	host = gethostbyname ("localhost");
  // connect to something outside of localhost
  //server.sin_addr.s_addr = inet_addr('ip_address');
  
	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}

	while (1) {

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

		read (s, &number, sizeof (number));
		close (s);
		fprintf (stderr, "Process %d gets number %d\n", getpid (),
			ntohl (number));
		sleep (5);
	}
}
