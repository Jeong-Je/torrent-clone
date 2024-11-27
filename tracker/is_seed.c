#include "is_seed.h"
#include "request.h"

bool is_seed(Request req, const char* file_path){

	char line[MAX_LEN_LENGTH];

	FILE* seedlist = fopen(file_path, "rb");	// 시드 리스트 파일 열기
	
	while(fgets(line, sizeof(line), seedlist)){	// 한줄씩 읽기
		
		strtok(line, "\n");
		if(strcmp(line, req.client_ip) == 0){			// 이미 시드가 존재하면 true

			return true;
		}
	}

	return false;
}