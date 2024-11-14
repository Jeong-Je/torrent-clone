#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "make_torrent.h"
#include "bdecode.h"
#include "meta.h"

int main(int argc, char *argv[]) {
    if (argc == 3) {
	char *option = argv[1]; 
        char *file_path = argv[2];

	// make 명령어
	if(strcmp(option, "make") == 0) {
        	create_torrent(file_path, "113.198.138.212");
	} else if(strcmp(option, "down") == 0) { // down 명령어
		meta meta;
		if(bdecode(&meta, file_path)){
			printf("announce = %s\n", meta.announce);
			printf("file_name = %s\n", meta.name);
			printf("file_length = %d\n", meta.file_length);
			printf("piece_length = %d\n", meta.piece_length);
			printf("piece_num = %d\n", meta.piece_num);
			printf("메타데이터 파싱 완료.");
		} else {
			fprintf(stderr, "메타데이터 파싱 실패.");
		}
	} else {
		fprintf(stderr, "인자 오류\n");
		return 1;
	}
    } else {
	printf("torrent client program manual\n");
        printf(".torrent 생성: %s make <파일 경로>\n", argv[0]);
	printf(".torrent 다운: %s down <파일 경로>\n", argv[0]);
        return 0;
    }
    return 0;
}

