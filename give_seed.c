#include "give_seed.h"

int give_seed(const char* file_id){

	//test
	printf("give seed method call \n");

	char line[1024];
	char* token;
	FILE* seedlist = fopen("seedlist.txt", "r");

	while(fgets(line, sizeof(line), seedlist)){
	
		// 시드 제공 알고리즘 적용 필요
		// file_id 전부 추출한다음 배열에 추가
		// 배열에서 순회와 랜덤 돌리기

		token = strtok(line, ":");
		if(strcmp(token, file_id) == 0){
		
			token = strtok(NULL, ":");
			printf("seed ip : %s\n", token);
		}
	}

	printf("There is not seeds\n");

	return 0;
}
