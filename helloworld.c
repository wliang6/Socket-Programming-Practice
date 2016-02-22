#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h> //after typing man socket (for manual)
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include<unistd.h>


int main()
{
	//Take a TCP socket and bind it to particular port. 
	//Wait for a client, send message "Hello World" to client, and close connection
	int sock, cli; //cli - client
	struct sockaddr_in server, client;
	unsigned int len;
	char msg[] = "Hello to the world of socket programming!";
	int sent; //# of bytes in message sent

	//SOCKET() -- initializes socket
	//socket(int domain, int type, int protocol);
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket: ");
		exit(-1);

	}

	server.sin_family = AF_INET; //address family
	server.sin_port = htons(10000); //byte ordering changed to network byte order; 10000 as port number in this example
	server.sin_addr.s_addr = INADDR_ANY; //ip address server binds to; binds to all; server should bind to all interfaces in the local machine
	bzero(&server.sin_zero, 8); //for porting, 8 bytes long


	//BIND() -- attaches ip address and port number 
	//bind(int socket, const struct sockaddr *address, socklen_t address_len);
	len = sizeof(struct sockaddr_in);
	if((bind(sock, (struct sockaddr *)&server, len)) == -1)
	{
		perror("socket: ");
		exit(-1);

	}
	
	//LISTEN() -- wait for client to connect to server
	//listen(int socket, int backlog);
	if((listen(sock, 5)) == -1)
	{
		perror("listen: ");
		exit(-1);
	}

	while(1)
	{
		//socket descriptor called cli
		//ACCEPT -- server accepts so client can connect to server
		//accept(int socket, struct sockaddr *restrict address,socklen_t *restrict address_len);
		if((cli = accept(sock, (struct sockaddr *)&client, &len)) == -1)
		{
			perror("accept: ");
			exit(-1);
		}
		//once client socket returns successfully, send out message to the client
		//SEND() -- transfers data and sends message
		//send(int socket, const void *buffer, size_t length, int flags);
		sent = send(cli, msg, strlen(msg), 0);
		//char * inet_ntoa(struct in_addr in);
		printf("Sent %d bytes to client: %s\n", sent, inet_ntoa(client.sin_addr));
		//CLOSE() -- closes connection to the client
		close(cli);
	}


}
