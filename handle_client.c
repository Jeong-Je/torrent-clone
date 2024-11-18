#include"handle_client.h"

int handle_client(char* command, int *new_socket, int* server_socket){

	char buf[MAX_BUF];
	pid_t pid = fork();

	if(pid == 0){
		
		close(*server_socket); //서버 소켓 닫기

		memset(buf, '\0', sizeof(buf));

		if(recv(*new_socket, buf, sizeof(buf), 0) == -1){

			perror("recv err"); exit(1);
		}	


		if(strcmp(buf, "give_seed") == 0){
		
			//시드 정보 요청 구현
		}else if(strcmp(buf, "iam_seed") == 0){
			
			//test
		} else{
		
			printf("please enter these command : give_seed or iam_seed");
			close(*new_socket);
			exit(1);
		}

		close(*new_socket); //자식 소켓 닫기

	} else if (pid > 0){
	
		wait(NULL);
		

	} else{
	
		perror("fork err"); exit(1);
	}

	return 0;
}
