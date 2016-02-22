/*
* Author: Winnie Liang
* Date: February 22, 2016
* A Simple TCP Echo Server for practice. Taken by securitytube.net video called
* "Tcp Echo Server Using Sockets".
* The purpose of a TCP Echo server is to echo back the client sends to it. 
* The client can send a string to server and server echoes back the same string
* back to client. At one point, echo servers were used for diagnostic purposes.
* The echo server in this program runs an infinite loop that process clients 
* serially one after the other.
* To add multi-client support, we will have to use the select() asynchronous call * or make it multithreaded. 
*/

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#include<strings.h>
#include<unistd.h>
#include<arpa/inet.h>

#define ERROR -1
#define MAX_CLIENTS 2
#define MAX_DATA 1024 //size of the buffer passed as send and receive

int main(int argc, char **argv)
{
	struct sockaddr_in server; //server structure contains port
	struct sockaddr_in client; //client structure contains client info containing to our server
	int sock; //socket descriptor
	int new;  //client socket descriptor - whenever client connects to server, server needs a new socket
 	unsigned int sockaddr_len = sizeof(struct sockaddr_in); //input for bind and accept calls
	int data_len;
	char data[MAX_DATA];
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
	{
		perror("server socket: ");
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1])); //convert from ASCII to int, then convert from host byte order to network byte order
	server.sin_addr.s_addr = INADDR_ANY; //instruct kernel to listen to all interfaces on our hosts/connections on the local machine 0.0.0.0
	bzero(&server.sin_zero, 8);

	//bind() to the port
	if((bind(sock, (struct sockaddr *)&server, sockaddr_len)) == ERROR)
	{
		perror("bind: ");
		exit(-1);
	}

	//listen() to same socket
	if((listen(sock, MAX_CLIENTS)) == ERROR)
	{
		perror("listen");
		exit(-1);
	}
	
	while(1) //better signal handling required to wait for client connection and start mechanism
	{
		//accept() is a blocking call is to wait for a connection; TIL the client is client, server would be blocked on its end until it receives connection
		//new is a socket descriptor for the client (send/receive on this)
		if((new = accept(sock, (struct sockaddr *) &client, &sockaddr_len)) == ERROR)	
		{
			perror("accept");
			exit(-1);
		}
		//prints the converted client port number (network byte order to host byte order) and address (network to ascii)
		printf("New Client connected from port no %d and IP %s\n", ntohs(client.sin_port), inet_ntoa(client.sin_addr));
		
		data_len = 1;
		//start a loop until client is connected to us
		while(data_len)
		{
			//first string data has to be initiated by client
			//recv() call on socket descriptor received by accept, data is where it is stored and size of storage

			//data_len - how many bytes of data sent by client, not 0 or -
			data_len = recv(new, data, MAX_DATA, 0);

			if(data_len) //there are data sent by client
			{
				//send() - issue back the same data to the client sent through same socket descriptor without modification
				send(new, data, data_len, 0);
				//null the last byte '\0' when printing. last byte in the data sent by client. When printing the string, printing requires that the string is over so you need this null. It will crash without THIS.
				data[data_len] = '\0';
				printf("Sent msg: %s", data);
			}
		}
		//the while loop segment above keeps going forever until client disconnects, recv fails and data_len becomes -1 or 0. It gets out of while loop
		printf("Client disconnected\n");

		close(new); //close this socket connected to client
		//everything above is still within a while loop. Once socket closes, the server goes back to accept() state from above and waits for new clients to connect to server 
	}
	close(sock);

}

//To compile, run gcc compiler on terminal and create an executable with the following:  gcc echoserver.c -o echoserver
//To run, ./server 10000 // gives port number 10000 
//To confirm this, open another terminal and type in: netstat -atn
//To connect to this client port, telnet localhost 10000

