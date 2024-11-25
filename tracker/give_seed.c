#include "give_seed.h"

int give_random(FILE* file, char file_full_path_name[256], char line[256], char* seeds[MAX_SEEDS], int seed_cnt){

	int seeds_index = 0;

	for(int i=0; i<5; i++){

		srand(time(0));
		int random_value = rand() % seed_cnt;	// 랜덤값 추출

		int temp = 0;
		while(temp > random_value && fgets(line, 256, file)){

			temp++;	// 랜덤한 줄으로 이동
		}

		fgets(line, 256, file);

		seeds[seeds_index++] = line;	//시드 선택
	}

	return 0;
}

void* give_seed(void* arg){

	char* file_id = (char*) arg;

	char line[256]; 
	char file_full_path_name[256];	// 요청들어온 파일의 경로와 이름
	char* seeds[MAX_SEEDS];			// 클라이언트에게 줄 시드들
	FILE* file; 	// 시드 리스트 관리할 파일 포인터

	snprintf(file_full_path_name, sizeof(file_full_path_name), "./seed_List/%s", file_id);
	
	file = fopen(file_full_path_name, "rb");
	if(file == NULL){

		perror("fopen err"); exit(1);
	}

	int seed_cnt = 0; 	//시드 개수
	while(fgets(line, sizeof(line), file)){		//시드 개수 세기

		seed_cnt++;
	}

	if(seed_cnt > 5){

		give_random(file, file_full_path_name, line, seeds, seed_cnt);	// 랜덤 시드
		//순회 시드 추후 구현필요
	} else if(seed_cnt == 0){

		printf("There is not seeds.");
	} else{

		int index = 0;
		memset(line, '\0', sizeof(line));	//line 비우기
		while(fgets(line, sizeof(line), file)){	// 시드개수가 0~5라면 그냥 주기

			seeds[index++] = line;	//시드 선택
		}
	}

	// 선택된 시드 출력 
	for(int i=0; i<5; i++){

		printf("selected seed %d: %s", i, seeds[i]);
	}

	return 0;
}