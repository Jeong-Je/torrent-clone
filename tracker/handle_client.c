#include"handle_client.h"

int handle_client(int *new_socket, int* server_socket){

	char buf[MAX_BUF];
	pid_t pid = fork();

	if(pid == 0){

		close(*server_socket);

		memset(buf, '\0', sizeof(buf));
		
		if(recv(*new_socket, buf, sizeof(buf) , 0) == -1){

			perror("recv err"); exit(1);
		}	

		if(strcmp(buf, "give_seed") == 0){
		
			//시드 정보 요청 구현
			printf("give seed \n");

		} else if(strcmp(buf, "iam_seed") == 0){
			
			//시드 리스트 추가 구현
			printf("iam seed\n");

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
