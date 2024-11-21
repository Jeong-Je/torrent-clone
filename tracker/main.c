#include <stdio.h>      // printf, perror 등의 표준 입출력 함수
#include <stdlib.h>     // exit 함수
#include <string.h>     // 문자열 관련 함수 (memset, memcpy 등)
#include <unistd.h>     // close, fork 등의 POSIX 함수
#include <arpa/inet.h>  // 주소 변환 함수 (inet_ntoa, htons 등)
#include <sys/types.h>  // 다양한 데이터 타입, pid_t 포함
#include <sys/socket.h> // 소켓 관련 함수 및 구조체
#include <netinet/in.h> // sockaddr_in 구조체

#define PORTNUM 32132
#define CLIENT_MAX  10

int main(){
	struct sockaddr_in server_address, client_address;
	int server_socket, new_socket;
	int clientlen = sizeof(client_address);

	char buf[1024];

	//socket 생성
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1){
		perror("socket");
		exit(1);
	}

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORTNUM);
	server_address.sin_addr.s_addr = inet_addr("0.0.0.0");

	// 바인딩
	if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address))) {
		perror("bind");
		close(server_socket);
		exit(1);
	}

	//listen
	if(listen(server_socket, CLIENT_MAX) < 0){
        perror("listen err");
        close(server_socket);
        exit(1);
    }

	while(1) {
		if((new_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&clientlen)) < 0) {
			perror("accept");
			exit(1);
		}

		if(recv(new_socket, buf, sizeof(buf), 0) == -1) {
			perror("recv");
			exit(1);
		}

		char *msg = strtok(buf, ":"); // ":" 문자를 기준으로 앞 문자열

		if(strcmp(msg, "iam_seed") == 0) {
			printf("iam_seed 요청 수신\n");
			printf("%s\n", msg);

			msg = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열
			printf("%s\n", msg);
		} else if(strcmp(msg, "give_seed") == 0) {
			printf("give_seed 요청 수신\n");
			printf("%s\n", msg);

			msg = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열
			printf("%s\n", msg);
		}
	}

	close(server_socket);
	close(new_socket);

	return 0;
}
