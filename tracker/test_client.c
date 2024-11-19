#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/un.h>
#include<unistd.h>

#define PORTNUM 12344

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

	char buf[256];
	
	printf("command : ");
	scanf("%s", buf);

	if(send(client_socket, buf, strlen(buf), 0) == -1){
	
		perror("send err"); exit(1);
	}

	char file_id[256];
	printf("file id : ");
	scanf("%s", file_id);

	if(send(client_socket, file_id, strlen(file_id), 0) == -1){
	
		perror("send err"); exit(1);
	}

	close(client_socket);
}
