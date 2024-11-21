#include "update_seedlist.h"

typedef struct { // 클라 요청에서 filename과 client_ip를 담을 구조체
	char file_name[100];
	char client_ip[16];
} Request;

void *update_seedlist(void *data){
	Request * req = (Request *)data;

    //printf("파일명 = %s, 클라이언트 IP = %s\n", req->file_name, req->client_ip);

	FILE *file; // 시드 리스트 관리할 파일 포인터

	// 피어 리스트를 관리하는 seed_List 폴더가 없을 경우 새로 생성함
	if(mkdir("./seed_List", 0755) == -1 && errno != EEXIST) {
		perror("디렉터리 생성 실패");
		return NULL;
	}

	// 파일 경로 생성
	char file_path[100];
	snprintf(file_path, sizeof(file_path), "./seed_List/%s", req->file_name);
	file = fopen(file_path, "ab"); // 파일 열자 ! 없으면 만들어~

	if(file == NULL){
		perror("파일 열기 실패");
		return NULL;
	}

	// 파일에 클라 ip 기록
	fprintf(file, "%s\n", req->client_ip);

	fclose(file);

	return NULL;
	
}
