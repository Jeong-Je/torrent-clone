#include "give_seed.h"

int give_seed(const char* file_id){

	char line[1024];
	FILE* seedlist = fopen("seedlist.txt", "r");

	while(fgets(line, sizeof(line), seedlist)){
	
		if(strstr(line, file_id) != NULL){
		
			// 시드 제공 알고리즘 적용 필요
		}
	}
}
