#include "update_seedlist.h"
#include "is_seed.h"

int update_seedlist(Request req){
    printf("파일명 = %s, 클라이언트 IP = %s 포트 = %d\n", req.file_name, req.client_ip, (req.port_num));

	FILE *file; // 시드 리스트 관리할 파일 포인터

	// 피어 리스트를 관리하는 seed_List 폴더가 없을 경우 새로 생성함
	if(mkdir("./seed_List", 0755) == -1 && errno != EEXIST) {
		perror("디렉터리 생성 실패");
		exit(1);
	}

	// 파일 경로 생성
	char file_path[1024];
	snprintf(file_path, sizeof(file_path), "./seed_List/%s", req.file_name);
	file = fopen(file_path, "ab"); // 파일 열자 ! 없으면 만들어~

	if(file == NULL){
		perror("파일 열기 실패");
		exit(1);
	}

	if(is_seed(req, file_path)){	// 이미 시드로 존재하는 경우

		printf("이미 시드입니다.\n");
	} else{	// 아직 아닌경우

		// 파일에 클라 ip 기록
		fprintf(file, "%s:%d\n", req.client_ip, req.port_num);
		printf("시드리스트가 갱신되었습니다.\n");

		if(send(req.new_socket, &(req.port_num), sizeof(req.port_num), 0) == -1){
			perror("send");
			exit(1);
		}
	}

	fclose(file);

	return 0;
}