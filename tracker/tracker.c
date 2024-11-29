#include <stdio.h>      // printf, perror 등의 표준 입출력 함수
#include <stdlib.h>     // exit 함수
#include <string.h>     // 문자열 관련 함수 (memset, memcpy 등)
#include <unistd.h>     // close, fork 등의 POSIX 함수
#include <arpa/inet.h>  // 주소 변환 함수 (inet_ntoa, htons 등)
#include <sys/types.h>  // 다양한 데이터 타입, pid_t 포함
#include <sys/socket.h> // 소켓 관련 함수 및 구조체
#include <netinet/in.h> // sockaddr_in 구조체

#include <pthread.h>

#include "request_thread.h"

#define PORTNUM 32132
#define CLIENT_MAX  10

typedef struct { 	// request_thread 함수로 넘길 인자 
	struct sockaddr_in client_address;
	int new_socket;
} Arg;

int main(){

	struct sockaddr_in server_address, client_address;
	int server_socket;
	int clientlen = sizeof(client_address);

	pthread_t tid; // tid 생성

	//socket 생성
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket == -1){
		perror("socket");
		exit(1);
	}

	// 포트 재사용
	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
			perror("setsockopt 실패");
			close(server_socket);
			exit(EXIT_FAILURE);
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
		Arg arg1;
		int *new_socket = malloc(sizeof(int));  // 동일한 소켓이 전달되지 않도록 동적 메모리 할당

		if((*new_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&clientlen)) < 0) {
			perror("accept");
			free(new_socket);
			exit(1);
		}

		arg1.new_socket = *new_socket;
		arg1.client_address = client_address;	//인자 세팅

		if(pthread_create(&tid, NULL, request_thread, (void*)&arg1) != 0){

			perror("pthread_create err");
			free(new_socket);
			exit(1);
		}	// 클라이언트 요청을 스레드생성해서 처리

		//pthread_join(tid, NULL);
		pthread_detach(tid);
	}

	close(server_socket);

	return 0;
}