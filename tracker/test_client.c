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
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("113.198.138.212");

	if(connect(client_socket, (struct sock_addr*)&client_address, sizeof(client_address)) == -1){
	
		perror("connect err"); exit(1);
	}

	//send 로직 구현필요 및 인자값도 넣어줘야됨
}
