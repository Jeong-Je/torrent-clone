#include "is_seed.h"

bool is_seed(const char* file_id, const char* client_ip){

	char line[MAX_LEN_LENGTH];
	
	char filename[1024];
	snprintf(filename, sizeof(filename), "%s_seedlist.txt", file_id);

	FILE* seedlist = fopen(filename, "r");
	if(seedlist == NULL){	//file이 존재하지 않으면 시드 리스트 갱신		
	
	       	return false;
	}
	
	while(fgets(line, sizeof(line), seedlist)){
	
		if(strcmp(line, client_ip) == 0){
			
			return true;
		}
	}

	return false;
}
