#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h> // basename 함수를 위해서 추가
#include <netinet/in.h>

#include "make_torrent.h"
#include "bdecode.h"
#include "meta.h"
#include "allocate_storage.h"
#include "download_piece.h"
#include "report_to_tracker.h"
#include "send_pieces.h"

int main(int argc, char *argv[]) {
 	if (argc == 1) {
        // Default behavior when no arguments are provided
        printf("torrent client program manual\n");
        printf(".torrent 생성: %s make <파일 경로>\n", argv[0]);
        printf(".torrent 다운: %s down <파일 경로>\n", argv[0]);

		send_pieces();

        return 0;
    }
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
	} else if(strcmp(option, "down") == 0) { // down 명령어
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

		// 트래커로부터 피어의수(1) + 피어의 IP(127.0.0.1)를 받음(일단 하드코딩)
		int peer_num = 1;
		if (peer_num == 0){
			perror("다운받을 수 없습니다.");
			exit(1);
		}
		in_addr_t seed_IP = inet_addr("127.0.0.1");

		// 분배할 조각의 수와 피어의 수에 따라 인덱스 나누기
		int index_term = meta.piece_num / peer_num;
		int addition_term = meta.piece_num % peer_num;

		int start_index = 0;
		int end_index;
		
		// 저장공간 할당
		char temp_file_name[256];
		allocate_storage(meta, temp_file_name);

		// 설정한 인덱스에 따라 다운로드 요청
		for (int i=0; i<peer_num; i++){
			end_index = start_index + index_term -1;
			if (i<addition_term){
				end_index++;
			}

			if (end_index >= meta.piece_num) {
        		end_index = meta.piece_num - 1;
   			}

			// thread로 수정 필요
			receive_piece(meta, start_index, end_index, seed_IP, temp_file_name);

			start_index = end_index+1;
		}

		// 다운로드 완료시 원래 파일 이름으로 변경
		if (rename(temp_file_name, meta.name) == 0) {
        printf("파일 이름이 성공적으로 변경되었습니다.");
		} else {
			perror("파일 이름 변경 실패");
			exit(1);
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

