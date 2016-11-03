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

#define MY_PORT 21252    // port we're listening on

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

    int BUFSIZE = 512;
    unsigned char buf[BUFSIZE];    // buffer for client data
    unsigned char sndbuf[BUFSIZE]; // buffer for received data
    unsigned int nbytes;
    unsigned char handbuf[2] = {0xCF, 0xA7};  // handshake bytes
    unsigned int n_users=0;
    unsigned char userlist[50][20];           //50 users, 20 in length

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j;

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
                          // printf("Sending userlist\n");
                          unsigned int userlen;
                          // loop through all fds associated with usernames
                          for(j = 0; j <= fdmax; j++) {
                              if (FD_ISSET(j, &master)) {
                                  // except the listener and ourselves
                                  if (j != listener && j != i ) {
                                    // prepare the username and send it off
                                    userlen=(unsigned int)userlist[j][0];
                                    printf(" uln len: %d\n", userlen);

                                    sndbuf[0]=userlist[j][0];
                                    for (i=1; i<=userlen; i++) {
                                      sndbuf[i]=userlist[j][i];
                                    }
                                    printBuf("send username",0,sndbuf,userlen);
                                    sendMessage(newfd, sndbuf, userlen);

                                  }
                              }
                          } // END send to everyone

                        } // END list of users

                        // ==== get new username====;
                        receiveMessage(newfd, buf,1);

                        unsigned int len = (unsigned int)buf[0];

                        printf("Get username of len %d\n", len);
                        receiveMessage(newfd, buf, len);

                        userlist[newfd][0]=(unsigned char)len;
                        for (int j=1; j<=len; j++) {
                          userlist[newfd][j]=buf[j-1];
                        }

                        printf("New user: %s\n", userlist[newfd]);

                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }

                        // Print out current user list for funsies
                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                // except the listener
                                if (j != listener ) {
                                  if (userlist[j][0]!=0) {
                                    printBuf("userlist +1",1,userlist[j],(unsigned int)userlist[j][0]);
                                  }
                                }
                            }
                        }
                        // ===========================

                        // ======== User Coonnected Broadcast ==========
                        // initialize status code
                        sndbuf[0]=0x01;

                        appendUser(sndbuf, 1, userlist[newfd]);


                        // send to everyone
                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i && j!=newfd ) {
                                    sendMessage(j,sndbuf,len+1); // account for extra code byte
                                    // printf("PRINT SNDBUF %c, %d, %s\n",(unsigned char)sndbuf[0], (unsigned int)sndbuf[1],sndbuf);
                                }
                            }
                        } // END send to everyone

                        n_users++;

                        printf("selectserver: new connection from %s:%d on socket %d\n",
                            inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port), newfd);
                    }
                } else {
                    // handle data from a client
                    nbytes = recv(i, buf, sizeof(unsigned char), 0);
                    if (nbytes <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // ====== User Disconnected  =======
                            printf("selectserver: socket %d hung up\n", i);
                            unsigned int len = (unsigned int)userlist[i][0];
                            printf("len %d\n",len);
                            // initialize status code
                            sndbuf[0]=0x02;
                            appendUser(sndbuf, 1, userlist[i]);


                            // send to everyone
                            for(j = 0; j <= fdmax; j++) {
                                if (FD_ISSET(j, &master)) {
                                    // except the listener and ourselves
                                    if (j != listener && j != i ) {

                                        sendMessage(j,sndbuf,len+1); // account for extra code byte
                                        // printf("PRINT SNDBUF %c, %d, %s\n",(unsigned char)sndbuf[0], (unsigned int)sndbuf[1],sndbuf);
                                    }
                                }
                            } // END send to everyone

                            // remove user from list
                            userlist[i][0] = 0;
                            // Print out current user list for funsies
                            for(j = 0; j <= fdmax; j++) {
                                if (FD_ISSET(j, &master)) {
                                    // except the listener
                                    if (j != listener ) {
                                      if (userlist[j][0]!=0) {
                                        printBuf("userlist -1",1,userlist[j],(unsigned int)userlist[j][0]);
                                      }
                                    }
                                }
                            }

                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        n_users--;
                    } else {
                        // we got some message from a client

                        // get the len of message
                        unsigned int numchar = (unsigned int) buf[0];
                        printf("Got: %d\n", numchar);

                        // get the message string
                        receiveMessage(i, buf, numchar);

                        // print to the server for debugging (-1 because no len byte)
                        printBuf("rcv message", 0, buf,numchar);

                        // prep message
                        sndbuf[0] = 0x00;  // chat code
                        unsigned int len = (unsigned int) userlist[i][0];
                        prepareMessage(sndbuf,1,userlist[i],len);
                        // prepareMessage(sndbuf,2+len,buf,numchar);
                        // send to everyone
                        for(j = 0; j <= fdmax; j++) {
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i ) {
                                    if (send(j, sndbuf, sizeof(unsigned char), 0) == -1) {
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
