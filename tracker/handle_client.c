#include "handle_client.h"
#include "update_seedlist.h"
#include "is_seed.h"

int handle_client(int *new_socket, struct sockaddr_in* client_address){

	char buf[MAX_BUF];
	char file_id[MAX_BUF];
	pid_t pid = fork();

	if(pid == 0){

		memset(buf, '\0', sizeof(buf));
		
		if(recv(*new_socket, buf, sizeof(buf) , 0) == -1){

			perror("recv err"); exit(1);
		}	

		//test
		printf("command : %s\n", buf);

		if(strcmp(buf, "give_seed") == 0){
		
			//시드 정보 요청 구현
			printf("give seed \n");

			recv_buf(new_socket, file_id);	//파일 id 받기
			printf("file id : %s\n", file_id);

			give_seed(file_id);		// 시드 정보 제공

		} else if(strcmp(buf, "iam_seed") == 0){
			
			//시드 리스트 추가 구현
			char client_ip[INET_ADDRSTRLEN]; // 클라이언트 id 받기
			inet_ntop(AF_INET, &(client_address->sin_addr), client_ip, INET_ADDRSTRLEN);
			printf("client ip:%s\n", client_ip);	// test

			recv_buf(new_socket, file_id);	//파일 id 받기

			printf("file id:%s\n", file_id);	//test 

			if(is_seed(file_id, client_ip)){
			
				printf("you are already seed\n");
			} else{
			
				update_seedlist(file_id, client_ip);
				printf("seedlist is updated\n");
			}

		} else {
		
			printf("please enter these command : give_seed or iam_seed\n");
		}

		close(*new_socket);

	} else if (pid > 0){
	
		wait(NULL);

	} else{
	
		perror("fork err"); exit(1);
	}

	return 0;
}

int recv_buf(int* new_socket, char* buf){

	memset(buf, '\0', MAX_BUF);

	if(recv(*new_socket, buf, MAX_BUF, 0) == -1){
	
		perror("recv err"); exit(1);
	}

	return 0;
}
