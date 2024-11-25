#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> // basename 함수를 위해서 추가

#include "make_torrent.h"
#include "bdecode.h"
#include "meta.h"
#include "allocate_storage.h"
#include "download_piece.h"
#include "report_to_tracker.h"

int main(int argc, char *argv[]) {
    if (argc == 3) {
	char *option = argv[1]; 
    char *file_path = argv[2];
	char torrent_path[256];

	// make 명령어
	if(strcmp(option, "make") == 0) {
        	create_torrent(file_path, &torrent_path);
			char* torrent_file = basename(torrent_path);
			
			if(report_to_tracker(torrent_file)){
				printf("트래커 서버로 보고 완료\n");
			}else {
				printf("트래커 서버로 보고 실패\n");
			} 

	} 
	
	else if(strcmp(option, "down") == 0) { // down 명령어
		meta meta;

		if(bdecode(&meta, file_path)){
			printf("announce = %s\n", meta.announce);
			printf("file_name = %s\n", meta.name);
			printf("file_length = %d\n", meta.file_length);
			printf("piece_length = %d\n", meta.piece_length);
			printf("piece_num = %d\n", meta.piece_num);
			printf("메타데이터 파싱 완료.\n");

		} else {
			fprintf(stderr, "메타데이터 파싱 실패.");
		}

		char temp_file_name[256];
		allocate_storage(meta, temp_file_name);
		printf("temp file name: %s\n", temp_file_name);
		char* piece_map = (char*)malloc(sizeof(char) * meta.piece_num);
		memset(piece_map, 0, sizeof(char)*meta.piece_num);

		for (int i=0; i<meta.piece_num; i++){
			if (piece_map[i] == 0){
			download_piece(meta, temp_file_name);
			piece_map[i] = 1;
			}
		}

		if (rename(temp_file_name, meta.name) == 0) {
        	printf("파일 이름이 성공적으로 변경되었습니다.");
	
		} else {
			perror("파일 이름 변경 실패");
			exit(1);
		}

	} 
	
	else {
		fprintf(stderr, "인자 오류\n");
		return 1;
	}

    else {
		printf("torrent client program manual\n");
        printf(".torrent 생성: %s make <파일 경로>\n", argv[0]);
		printf(".torrent 다운: %s down <파일 경로>\n", argv[0]);
        return 0;
    }
	
    return 0;
}

