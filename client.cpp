//#include <stdio.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include <string.h>
//#define PORT 8080
//
//int main(int argc, char const *argv[])
//{
//	int sock = 0, valread;
//	struct sockaddr_in serv_addr;
//	char *hello = "Hello from client";
//	char buffer[1024] = {0};
//	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//	{
//		printf("\n Socket creation error \n");
//		return -1;
//	}
//
//	serv_addr.sin_family = AF_INET;
//	serv_addr.sin_port = htons(PORT);
//
//	// Convert IPv4 and IPv6 addresses from text to binary form
//	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
//	{
//		printf("\nInvalid address/ Address not supported \n");
//		return -1;
//	}
//
//	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//	{
//		printf("\nConnection Failed \n");
//		return -1;
//	}
//	send(sock , hello , strlen(hello) , 0 );
//	printf("Hello message sent\n");
//	valread = read( sock , buffer, 1024);
//	printf("%s\n",buffer );
//	return 0;
//}

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <csignal>


char message[] = "Hello there!\n";
char buf[sizeof(message)];

int main()
{
	int sock;
	struct sockaddr_in addr;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0)
	{
		perror("socket");
		exit(1);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425); // или любой другой порт...
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("connect");
		exit(2);
	}

	send(sock, message, sizeof(message), 0);
	recv(sock, buf, sizeof(message), 0);

	std::cout << buf << std::endl;
	close(sock);

	return 0;
}