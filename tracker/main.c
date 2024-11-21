#include <stdio.h>      // printf, perror 등의 표준 입출력 함수
#include <stdlib.h>     // exit 함수
#include <string.h>     // 문자열 관련 함수 (memset, memcpy 등)
#include <unistd.h>     // close, fork 등의 POSIX 함수
#include <arpa/inet.h>  // 주소 변환 함수 (inet_ntoa, htons 등)
#include <sys/types.h>  // 다양한 데이터 타입, pid_t 포함
#include <sys/socket.h> // 소켓 관련 함수 및 구조체
#include <netinet/in.h> // sockaddr_in 구조체

#include <pthread.h>

#include "update_seedlist.h"

#define PORTNUM 32132
#define CLIENT_MAX  10

typedef struct { // 클라 요청에서 filename과 client_ip를 담을 구조체
	char file_name[100];
	char client_ip[16];
} Request;

int main(){
	struct sockaddr_in server_address, client_address;
	int server_socket, new_socket;
	int clientlen = sizeof(client_address);

	char buf[1024];

	pthread_t tid; // tid 생성

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

	Request *req = malloc(sizeof(Request)); // file_name과 클라이언트 ip를 담을 구조체

	while(1) {
		if((new_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&clientlen)) < 0) {
			perror("accept");
			exit(1);
		}

		if(recv(new_socket, buf, sizeof(buf), 0) == -1) {
			perror("recv");
			exit(1);
		}

		char *msg = strtok(buf, ":"); // ":" 문자를 기준으로 앞 문자열 (command)
		if(strcmp(msg, "iam_seed") == 0) {
			printf("iam_seed 요청 수신\n");

			msg = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열  (.torrent 파일 이름)

			// 문자열 복사
			strncpy(req->file_name, msg, sizeof(req->file_name) - 1);
			strncpy(req->client_ip, inet_ntoa(client_address.sin_addr), sizeof(req->client_ip) -1);

			// 널 종료 문자 추가
			req->file_name[sizeof(req->file_name) - 1] = '\0';
			req->client_ip[sizeof(req->client_ip) - 1] = '\0';

			//printf("file_name: %s\n", req->file_name);
			//printf("client_ip: %s\n", req->client_ip);

			pthread_create(&tid, NULL, update_seedlist, (void *)req);
		} else if(strcmp(msg, "give_seed") == 0) {
			printf("give_seed 요청 수신\n");

			msg = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열 (.torrent 파일 이름)
		}
	}

	free(req); // 구조체 메모리 반환

	close(server_socket);
	close(new_socket);

	return 0;
}
