#include "request_thread.h"
#include "update_seedlist.h"
#include "give_seed.h"
#include "request.h"

typedef struct { 	// request_thread 함수로 넘길 인자 
	struct sockaddr_in client_address;
	int new_socket;
	int base_port_num;
} Arg;

void* request_thread(void* data){

	char buf[512];
    //pthread_t tid;
    Arg* arg1 = (Arg*)data;
	printf("socket : %d\n", arg1->new_socket);
	if(recv(arg1->new_socket, buf, sizeof(buf), 0) == -1) {
		perror("recv");
		//free(&arg1->new_socket);
		exit(1);
	}

	Request req; // file_name과 클라이언트 ip를 담을 구조체

	char *msg = strtok(buf, ":"); // ":" 문자를 기준으로 앞 문자열 (command)
	if(strcmp(msg, "iam_seed") == 0) {		
		printf("iam_seed 요청 수신\n");	//test

		msg = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열  (.torrent 파일 이름)

		// 문자열 복사
		strncpy(req.file_name, msg, sizeof(req.file_name) - 1);
		strncpy(req.client_ip, inet_ntoa(arg1->client_address.sin_addr), sizeof(req.client_ip) -1);

		// 널 종료 문자 추가
		req.file_name[sizeof(req.file_name) - 1] = '\0';
		req.client_ip[sizeof(req.client_ip) - 1] = '\0';
		req.port_num = arg1->base_port_num;
		req.new_socket = arg1->new_socket;

		update_seedlist(req);

	} else if(strcmp(msg, "give_seed") == 0) {

		printf("give_seed 요청 수신\n");	// test

		char* file_name = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열 (.torrent 파일 이름)
		printf("file_name : %s\n", file_name); //test

		give_seed(file_name, arg1->new_socket);

	} else{

		printf("입력 형식을 확인해주세요.\n");
	}

	//close(arg1->new_socket);

    return NULL;
}