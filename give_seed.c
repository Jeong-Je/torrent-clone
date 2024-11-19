#include "give_seed.h"

int give_seed(const char* file_id){

	//test
	printf("give seed method call \n");

	char line[1024];
	char filename[1024];
	int seed_count = 0;

	snprintf(filename, sizeof(filename), "%s_seedlist.txt", file_id);
	FILE* seedlist = fopen(filename, "r");
	if(seedlist == NULL){
	
		printf("There is not seeds\n");
		return 0;
	}

	while(fgets(line, sizeof(line), seedlist)){
	
		// 시드 제공 알고리즘 적용 필요
		// 배열에서 순회와 랜덤 돌리기

		printf("seed ip : %s", line);

		seed_count++;
	}

	give_seed_random(seed_count, line, seedlist);

	if(seed_count == 0){ printf("There is not seeds\n"); }

	return 0;
}

int give_seed_random(int seed_count, char line[], FILE* seedlist){

	srand(time(NULL));
	int random_value = rand() % seed_count;

	printf("random value : %d\n", random_value);

	for(int i=0; i<random_value; i++){
	
		fgets(line, MAX_SIZE, seedlist);
	}

	printf("selected seed ip : %s\n", line);
	
	return 0;
}
