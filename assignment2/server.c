/*
  TODO:
    - from commandline: server379 portnumber
    - make daemon
    - handshake (receive connection -> send hex -> receive username)
    - handle multiple client inputs
    - chat UI (broadcast chat, update user status, communication errors)
    - logfile sever379procid.log
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "chathandler.h"

#define MY_PORT 2222   // port we're listening on

void handleUserChat(int s, char *buf){
  // receive msg len
  unsigned int numchar = (unsigned int) buf[0];
  printf("numchar: %d\n", numchar);
  unsigned int nbytes=0;
  while (nbytes<numchar) {
    nbytes += recv(s,buf,sizeof(unsigned char)*numchar,0);
  }

  printf("char: %s\n",buf);
  }

void handleUserDC(){};
void handleUserCon(){};


void parseMessage(int s, char *buf) {
  switch(buf[0]) {
    case (unsigned char)0x00 :
      printf("== Received 0x00 ==\n");
      break;
    case (unsigned char)0x01 :
      handleUserCon();
      break;
    case (unsigned char)0x02 :
      handleUserDC();
      break;
  }
}

int main(void)
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_in sa;
    struct sockaddr_in remoteaddr; // client address
    socklen_t addrlen;

    unsigned char buf[512];    // buffer for client data
    unsigned char sndbuf[512]; // buffer for received data
    int nbytes;
    unsigned char handbuf[2] = {0xCF, 0xA7};  // handshake bytes
    unsigned int n_users=0;
    unsigned char userlist[20][20];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    listener = socket(AF_INET, SOCK_STREAM, 0);

    // get us a socket and bind it
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port = htons(MY_PORT);

    if (bind(listener, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind");
    	exit(-1);
    }

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(-1);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one


    // TODO fork for reading??
    // main loop
    while(1) {

        // Check idle clientfds


        // check for things
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(-1);
        }

        // TODO fork child for handling writing?

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {

            if (FD_ISSET(i, &read_fds)) { // we got one!!

              // handle new connections
              if (i == listener) {

                    // Accept new connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        // ===== Send handshake ====

	                      if(send(newfd, handbuf, sizeof(handbuf), 0) == -1) {
				                    perror("handshake send");
		                    }
                        printf("Send handshake\n");

                        // ==== Send number of users
                        printf("Current user count %d\n",n_users);
                        buf[0] = (unsigned char)n_users;
                        if(send(newfd, buf, sizeof(unsigned char), 0) == -1) {
                            perror("n_users send");
                        }
                        // send list of users
                        if (n_users >0 ) {
                          printf("Sending userlist\n");
                          int k;
                          unsigned int userlen;
                          for (k=0; k < n_users; k++) {
                            userlen=(unsigned int)userlist[k][0];
                            printf(" uln len: %d\n", userlen);

                            sndbuf[0]=userlist[k][0];
                            for (i=1; i<=userlen; i++) {
                              sndbuf[i]=userlist[k][i];
                            }
                            printBuf(sndbuf,userlen);
                            sendMessage(newfd, sndbuf, userlen);
                            // if (send(newfd, sndbuf, userlen, 0) == -1) {
                            //     perror("userlist send");
                            // }
                          }
                        } // END list of users

                        // ==== get new username====;
                        receiveMessage(newfd, buf,1);

                        unsigned int len = (unsigned int)buf[0];
                        printf("Get username of len %d\n", len);
                        receiveMessage(newfd, buf, len);

                        userlist[n_users][0]=(unsigned char)len;
                        for (int j=1; j<=len; j++) {
                          userlist[n_users][j]=buf[j-1];
                        }

                        printf("New user: %s\n", userlist[n_users]);
                        for (int l=0; l<n_users+1;l++) {
                          printBuf(userlist[l],20);
                        }
                        n_users++;
                        // ====================
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }

                        // TODO
                        // ======== New User Connected Broadcast ==========
                        printf("selectserver: new connection from %s:%d on socket %d\n",
                            inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port), newfd);
                    }
                } else {
                    // handle data from a client
                    nbytes = recv(i, buf, sizeof(unsigned char), 0);
                    if (nbytes <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);

                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        n_users--;
                    } else {
                        // we got some message from a client
                        printf("Got: %d ", (unsigned int)buf[0]);

                        // get the len
                        unsigned int numchar = (unsigned int) buf[0];
                        printf("numchar: %d\n", numchar);

                        // get the message string
                        nbytes = recv(i,buf,sizeof(unsigned char)*numchar,0);

                        // print to the server for debugging
                        for (int i=1; i<=nbytes; i++) {
                          printf("%c",buf[i-1]);
                        } printf("\n");

                        // send to everyone
                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i ) {
                                    if (send(j, buf, nbytes, 0) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        } // END send to everyone

                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END while

    return 0;
}
