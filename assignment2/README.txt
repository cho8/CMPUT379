CMPUT379
Fall 2016
Christina Ho
Cristian Dumitrescu

This is a simple chat service written in C. Files included in this submission:

client.c
server.c
chathandler.h
makefile

COMPILATION:
To compile both executables:
	make

To compile just the client:
	make client

To compile just the server:
	make server

To remove executables and object files:
	make clean


RUNNING EXECUTABLES:
To run the server:
	./server379 <port>
where <port> is the port number the client can connect to.

To run the client:
	./client379 <ip address> <port> <username>
where <ip address> is the ip address and the <port> of the server you want to connect to and <username> is name you want to use in the chat service.
If the username is not unique to the server, you will be disconnected.

UI:
While in the chat server, simply type and hit enter to send a message.
To leave the service, type '!exit' or hit Ctrl+c.


