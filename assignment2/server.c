/*
  TODO:
    X from commandline: server379 portnumber
    X make daemon
    X handshake (receive connection -> send hex -> receive username)
    X handle multiple client inputs
    X chat UI (broadcast chat, update user status, communication errors)
    X logfile server379procid.log
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#include "chathandler.h"

int fdmax;        // maximum file descriptor number
int listener;     // listening socket descriptor
fd_set master;    // master file descriptor list

FILE *fp=NULL;
struct sigaction sigint_act, oldint_act;
struct sigaction sigterm_act, oldkill_act;

void sigterm_handler(int signo){
  fprintf(fp, "SIGTERM received. Server closing\n");
  sigaction(SIGTERM, &sigterm_act, &oldkill_act);

  // close the sockets
  int j;
  for(j = 0; j <= fdmax; j++) {
      if (FD_ISSET(j, &master)) {
          // except the listener
          // prepare the username and send it off
          close(j);
          FD_CLR(j, &master); // remove from master set
      }
  } // END send to everyone
  fprintf(fp, "Close successful.\n");
  printf("Connections closed.\n");
  fclose(fp);
  exit(0);
}



int broadcastStatus (unsigned char code, int i, int newfds, unsigned char userlist[], unsigned char* sndbuf, fd_set master, int listener, int fdmax) {
  int j;
  unsigned int len = (unsigned int)userlist[0];
  // initialize status code
  sndbuf[0]=code;
  appendFrag(sndbuf, 1, (unsigned int)userlist[0], userlist);

  // Tell everyone
  for(j = 0; j <= fdmax; j++) {
      if (FD_ISSET(j, &master)) {
          // except the listener and ourselves
          if (j != listener && j != i  && j != newfds) {
              if(sendMessage(j,sndbuf,len+1) ==-1) { // account for extra code byte
                fprintf(fp, "[Error] Code %x from %d. Broadcast to %d.\n", code, i,j);
              }
          }
      }
  } // END send to everyone
  return 0;

}


int main(int argc, char *argv[])
{

    fd_set read_fds;  // temp file descriptor list for select()
    int newfd;        // newly accept()ed socket descriptor

    struct sockaddr_in sa;
    struct sockaddr_in remoteaddr; // client address
    socklen_t addrlen;

    int BUFSIZE = 512;
    int NUMUSERS = 50;
    int USERNAME_MAX = 20;
    unsigned char buf[BUFSIZE];    // buffer for client data
    unsigned char sndbuf[BUFSIZE]; // buffer for received data
    unsigned int nbytes;
    unsigned char handbuf[2] = {0xCF, 0xA7};  // handshake bytes
    unsigned int n_users=0;
    unsigned char userlist[NUMUSERS][USERNAME_MAX];           //50 users, 20 in length

    struct timeb timeouts[NUMUSERS];
    struct timeb current;

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j;

    // creating daemon process
    pid_t pid = 0;
    pid_t sid = 0;
    pid = fork();

    if (pid <0) {
      printf("[Error] Server failed to create daemon process.\n");
      exit(1);
    }

    if (pid > 0) {
      // in the parent
      printf("[PID] Daemon process %d \n", pid);
      exit(0);
    }

    umask(0);

    // don't look like an orphan!
    sid = setsid();
    if(sid < 0) {
    	fprintf(fp, "cannot create new process group");
      exit(1);
    }

    pid=getpid();

    char logbuf[128];
    sprintf(logbuf, "server379%d.log", pid);
    fp = fopen (logbuf, "w+");
    if(!fp){
      printf("[Error] Cannot open log file");
    }

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    listener = socket(AF_INET, SOCK_STREAM, 0);

    if (argc != 2) {
      printf("Usage: %s <portnumber> \n", argv[0]);
      exit (0);
    }

    //Setting up signal handler for killing daemon
    sigterm_act.sa_handler = sigterm_handler;
    sigemptyset(&sigterm_act.sa_mask);
    sigterm_act.sa_flags = 0;

    sigaction(SIGTERM, &sigterm_act, &oldkill_act);


    // get us a socket and bind it
    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa.sin_port = htons(atoi(argv[1]));

    if (bind(listener, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
      fprintf(fp, "[Error] Bind : %s\n",strerror(errno));
      perror("[Error] bind");
    	exit(1);
    }

    // listen
    if (listen(listener, 10) == -1) {
        fprintf(fp, "[Error] Listener : %s\n",strerror(errno));
        exit(1);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    fprintf(fp, "Server established with pid %d.\n",pid);
    fprintf(fp,	"Listening for connections.\n"\
		"Use 'kill %d' to end server connections.\n", pid);
    // main loop
    while(1) {

        // check for things
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, 0) == -1) {
            fprintf(fp, "[Error] Select : %s\n",strerror(errno));
            exit(1);
        }


        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {

            if (FD_ISSET(i, &read_fds)) { // we got one!!

              // handle new connections
              if (i == listener) {

                    // Accept new connection
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

                    if (newfd == -1) {
                        fprintf(fp, "[Error] Accept : %s\n",strerror(errno));
                    } else {

                        // ===== Send handshake ====

	                if(send(newfd, handbuf, sizeof(handbuf), 0) == -1) {
                            fprintf(fp, "[Error] Handshake send : %s\n",strerror(errno));
		        }
			fprintf(fp,"Sending handsake to %d\n",newfd);
                        // ==== Send number of users ====

                        buf[0] = (unsigned char)n_users;
                        if(send(newfd, buf, sizeof(unsigned char), 0) == -1) {
                            fprintf(fp, "[Error] n_users send: %s\n",strerror(errno));
                        }
                        // ==== send list of users ====
                        if (n_users >0 ) {

                          unsigned int userlen;
                          // loop through all fds associated with usernames
                          for(j = 0; j <= fdmax; j++) {
                              if (FD_ISSET(j, &master)) {
                                  // except the listener and ourselves
                                  if (j != listener && j != i ) {
                                    // prepare the username and send it off
                                    userlen=(unsigned int)userlist[j][0];

                                    sndbuf[0]=userlist[j][0];
                                    for (i=1; i<=userlen; i++) {
                                      sndbuf[i]=userlist[j][i];
                                    }

                                    if(sendMessage(newfd, sndbuf, userlen)==-1) {
                                      fprintf(fp, "[Error] Send userlist entry : %s\n",strerror(errno));
                                    }

                                  }
                              }
                          } // END send to everyone

                        } // END list of users

                        // ==== get new username====;
                        if(receiveMessage(newfd, buf,1)==-1) {
                          fprintf(fp, "[Error] Receive new user len : %s\n",strerror(errno));
                        }

                        unsigned int len = (unsigned int)buf[0];

                        if (receiveMessage(newfd, buf, len)==-1) {
                          fprintf(fp, "[Error] Receive new user : %s\n",strerror(errno));
                        }

                        userlist[newfd][0]=(unsigned char)len;
                        for (j=1; j<=len; j++) {
                          userlist[newfd][j]=buf[j-1];
                        }

                        // ==== set time out data
                        ftime(&timeouts[newfd]);

                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }

                        // ======== User Connected Broadcast ==========
                        broadcastStatus (0x01, i, newfd, userlist[newfd], sndbuf, master, listener, fdmax);

                        n_users++;

                        fprintf(fp, "[Connect] : new connection from %s:%d on socket %d\n",
                          inet_ntoa(remoteaddr.sin_addr), ntohs(remoteaddr.sin_port), newfd);
                    }
                } else {
                    // handle data from a client
                    nbytes = recv(i, buf, sizeof(unsigned char), 0);
                    if (nbytes <= 0) {
                        // got error or connection closed by client
                        // n_users = handleUserDC(nbytes, i, userlist[i], sndbuf, master, listener, fdmax, nbytes);
                        if (nbytes == 0) {
                            //====== User Disconnected  =======
                            fprintf(fp, "[Connect] : socket %d hung up\n", i);
                            broadcastStatus (0x02, i, i,userlist[i], sndbuf, master, listener, fdmax);
                            // remove user from list
                            userlist[i][0] = 0;
                        } else {
                            fprintf(fp, "[Error] Connection : %s\n",strerror(errno));
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        n_users--;
                    } else {
                        // we got some message from a client

                        // check timeout
                        ftime(&current);

                        if((current.time-timeouts[i].time) > TIMEOUT) {
                          // timeout threshold exceeded!
                          fprintf(fp, "[Connect] : socket %d timed out\n", i);
                          broadcastStatus (0x02, i, i, userlist[i], sndbuf, master, listener, fdmax);

                          // remove user from list
                          close(i);
                          userlist[i][0] = 0;
                          FD_CLR(i, &master); // remove from master set
                          n_users--;

                          continue;
                        } // otherwise it's alive still

                        ftime(&timeouts[i]);

                        // prep message
                        sndbuf[0] = 0x00;  // chat code
                        unsigned int len = (unsigned int)userlist[i][0]; // length of user
                        len = appendFrag(sndbuf, 1, len, userlist[i]); // attach user

                        unsigned int numchar = (unsigned int) buf[0];

                        if (numchar==0) {
                          // it's a keepalive!
                          ftime(&timeouts[i]);
                        } else {

                          len = appendFrag(sndbuf,len+1, 1, buf);       // attach message length
                          if (receiveMessage(i, buf, numchar)==-1) {
                            fprintf(fp, "[Error] Receive message len : %s\n",strerror(errno));
                          }
                          len = appendFrag(sndbuf, len, numchar, buf);  // attach message


                          // send to everyone
                          for(j = 0; j <= fdmax; j++) {
                              if (FD_ISSET(j, &master)) {
                                  // except the listener and ourselves
                                  if (j != listener ) {

                                      if (sendMessage(j, sndbuf, len)== -1) {
                                        fprintf(fp, "[Error] Forward chat : %s\n",strerror(errno));
                                      }

                                  }
                              }
                          } // END send to everyone
                        } // END >0 message
                     }
                } // END handle data from client

            } // END got new incoming connection
        } // END looping through file descriptors
    } // END while

    return 0;
}
