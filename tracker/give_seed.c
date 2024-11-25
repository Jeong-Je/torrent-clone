#include "give_seed.h"

int give_random(char file_path[256], char* seeds[MAX_SEEDS], int seed_cnt){

	FILE* file;
	char line[256];
	int seeds_index = 0;

	file = fopen(file_path, "rb");
	if(file == NULL){

		perror("fopen err"); exit(1);
	}

	bool* gived_seed = malloc(sizeof(bool) * seed_cnt + 1);		// 이미 준 시드 확인하는 배열
	gived_seed[0] = true;
	for(int i=1; i<seed_cnt + 1; i++) gived_seed[i] = false;

	srand(time(0));
	for(int i=0; i<5; i++){

		int random_value = rand() % seed_cnt + 1;	// 1~seed_cnt 범위로 랜덤한 값 추출
		while(gived_seed[random_value]){			// 안 뽑은 랜덤 값 추출

			random_value = rand() % seed_cnt + 1;	
		}


		int temp = 1;
		while(temp < random_value && fgets(line, 256, file)){

			temp++;	// 랜덤한 줄으로 이동
		}

		fgets(line, 256, file);

		seeds[seeds_index++] = line;				// 시드 선택
		gived_seed[random_value] = true;			// 이미 준 시드 체크
		printf("selected seed %d : %s", i, line);	// 시드 출력

		memset(line, '\0', sizeof(line));	//line 비우기
		fseek(file, 0, SEEK_SET);	// 파일 포인터 옮기기
	}

	return 0;
}

void* give_seed(void* data){

	printf("give_seed 함수 호출\n");

	char* file_name = (char*) data;
	//printf("filename:%s\n", file_name); //test

	char line[256]; 
	char file_path[256];			// 시드리스트의 경로
	char* seeds[MAX_SEEDS];			// 클라이언트에게 줄 시드들
	FILE* file; 					// 시드 리스트 관리할 파일 포인터

	snprintf(file_path, sizeof(file_path), "./seed_List/%s", file_name); 
	
	file = fopen(file_path, "rb");
	if(file == NULL){

		printf("There is not seeds.\n");
	}

	int seed_cnt = 0; 	//시드 개수
	while(fgets(line, sizeof(line), file)){		//시드 개수 세기

		//printf("line : %s", line);	//test
		seed_cnt++;
	}

	printf("seed_cnt : %d\n", seed_cnt);	//test

	if(seed_cnt > 5){

		give_random(file_path, seeds, seed_cnt);	// 랜덤 시드
		//순회 시드 추후 구현필요
	} else if(seed_cnt == 0){

		printf("There is not seeds.\n");
	} else{
		
		int index = 0;
		memset(line, '\0', sizeof(line));	//line 비우기
		fseek(file, 0, SEEK_SET);	// 파일 포인터 옮기기

		for(int i=0; i<seed_cnt; i++){

			while(fgets(line, sizeof(line), file)){	// 시드개수가 0~5라면 그냥 주기

				seeds[index++] = line;	// 시드 선택
				printf("selected seed %d : %s", i, line); // 시드 출력
			}
		}
	}

	fclose(file);

	return 0;
}

// 현재로서는 시드 콘솔에 출력까지만 해놓음

// 시드 선택해서 seeds 배열에 넣어놨고 해당 시드들을 클라이언트에게 어떻게 제공할지에 대한 코드 구현 필요