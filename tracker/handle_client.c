#include"handle_client.h"

int handle_client(int *new_socket){

	char buf[MAX_BUF];
	pid_t pid = fork();

	if(pid == 0){
		
		if(recv(*new_socket, buf, sizeof(buf) , 0) == -1){

			perror("recv err"); exit(1);
		}	

		//test
		printf("recv msg : %s\n", buf);

		if(strcmp(buf, "give_seed") == 0){
		
			//시드 정보 요청 구현
		}
		
		if(strcmp(buf, "iam_seed") == 0){
			
			//시드 리스트 추가 구현
		}

	} else if (pid > 0){
	
		wait(NULL);
		

	} else{
	
		perror("fork err"); exit(1);
	}



	return 0;
}
