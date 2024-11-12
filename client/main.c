#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "make_torrent.h"

int main(int argc, char *argv[]) {
    if (argc == 3) {
	char *option = argv[1]; 
        char *file_path = argv[2];
	if(strcmp(option, "make") == 0) {
        	create_torrent(file_path, "113.198.138.212");
	} else if(strcmp(option, "down") == 0) {
		printf("파일 다운\n");
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

