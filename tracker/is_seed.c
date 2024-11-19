#include "is_seed.h"

bool is_seed(const char* file_id, const char* client_ip){

	char line[MAX_LEN_LENGTH];

	FILE* seedlist = fopen("seedlist.txt", "r");
	
	while(fgets(line, sizeof(line), seedlist)){
	
		if(strstr(line, client_ip) != NULL){
		
			char* token = strtok(line, ":");
			if(strcmp(token, file_id) == 0){
			
				return true;
			}
		}
	}

	return false;
}
