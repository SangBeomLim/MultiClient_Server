/* comm proc */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

#include "Define.h"



//void* client_thread(CLIENT *cPtr);
#define FDCNT 5 //socket fd count

typedef struct {
	int fd;
	char ip[20];
}Connector;

int clientCount = 0;		//connect number
int sockfd_client[FDCNT];	//client_socket
Connector client_data[FDCNT] = {0};

extern "C"
{
void* client_thread(void* arg);
}

int main()
{
	int i;
	int acceptfd = 0;
	socklen_t client_addr_size = 0;
	int sockfd_server;
//	char msg[128];
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;

	pthread_t ptid[FDCNT] = {0};

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(9000);

	if((sockfd_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket() : ");
		return 0;
	}

	if(bind(sockfd_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("bind() : ");
		close(sockfd_server);
		return 0;
	}

	if(listen(sockfd_server, 5) == -1)
	{
		perror("listen() : ");
		close(sockfd_server);
		return 0;
	}

	client_addr_size = sizeof(client_addr);

	while(1)
	{
		acceptfd = accept(sockfd_server, (struct sockaddr*)&client_addr, &client_addr_size);

		if(clientCount == FDCNT)
		{
			printf("sockfd full\n");
			close(acceptfd);
			continue;
		}

		if(acceptfd < 0)
		{
			perror("accept() : ");
			continue;
		}

		for(i = 0; i <FDCNT; i++)
		{
			if(client_data[i].fd == 0)
			{
				client_data[i].fd = acceptfd;
				break;
			}
		}

		strcpy(client_data[i].ip, inet_ntoa(client_addr.sin_addr));
		pthread_create((ptid + i), 0, &client_thread, (void*)&i);

		clientCount++;
		printf("accept sockfd : %d, clientCount : %d i: %d\n", client_data[i].fd, clientCount, i);

	}
	close(sockfd_server);
	return 0;
}

void* client_thread(void* arg)
{

	int* i = (int*)arg;
	Connector* ptr = client_data + *i;
	int rst;
	char msg[128];
	while(1)
	{
		rst = read(ptr->fd, msg, sizeof(msg));
		if(rst <= 0)
		{
			break;
		}
		else
		{
			printf("[%s] : %s\n", ptr->ip, msg);
		}

		write(ptr->fd, msg, strlen(msg) + 1);
	}
	close(ptr->fd);
	clientCount--;
	ptr->fd = 0;
	printf("close(%s) \n", ptr->ip);
	return nullptr;
}


//using server.c
//void* client_thread(CLIENT* ptr)
//{
//	int rst;
//	char msg[128];
//	while(1)
//	{
//		rst = read(ptr->fd, msg, sizeof(msg));
//		if(rst <= 0)
//		{
//			break;
//		}
//		else
//		{
//			printf("[%s] : %s\n", ptr->ip, msg);
//		}
//
//		write(ptr->fd, msg, strlen(msg) + 1);
//	}
//	close(ptr->fd);
//	clientCount--;
//	ptr->fd = 0;
//	printf("close(%s) \n", ptr->ip);
//
//	return NULL;
//}
