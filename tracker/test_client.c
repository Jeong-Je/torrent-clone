#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/un.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

#define PORTNUM 32132

int main(){

	struct sockaddr_in client_address;
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(client_socket == -1){
	
		perror("socket create err"); exit(1);
	}

	memset((char*)&client_address, '\0', sizeof(client_address));
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(PORTNUM);
	client_address.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(client_socket, (struct sockaddr*)&client_address, sizeof(client_address)) == -1){
	
		perror("connect err"); exit(1);
	}

	printf("connect success\n");

	char buf[512];
	
	printf("please input command:fileId ex) iam_seed:12345\n input->");
	scanf("%s", buf);

	if(send(client_socket, buf, strlen(buf), 0) == -1){
	
		perror("send err"); exit(1);
	}


	close(client_socket);
}