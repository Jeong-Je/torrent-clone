#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <libgen.h> // basename 함수를 위해서 추가
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <sys/time.h> // gettimeofday 시간 측정을 위해서 추가

#include "receive_piece.h"
#include "make_torrent.h"
#include "bdecode.h"
#include "meta.h"
#include "allocate_storage.h"
#include "report_to_tracker.h"
#include "send_pieces.h"
#include "receive_piece.h"
#include "request_tracker.h"

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
        	create_torrent(file_path, torrent_path);
			char* torrent_file = basename(torrent_path);
			
			if(report_to_tracker(torrent_file)){
				printf("트래커 서버로 보고 완료\n");
			}else {
				printf("트래커 서버로 보고 실패\n");
			} 
	} else if(strcmp(option, "down") == 0) { // down 명령어
		struct timeval start, end; // 시간 변수
		gettimeofday(&start, NULL); // 시작 시간

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

		int peer_num = 0;
		char** seed_IP_arr;
		seed_IP_arr = request_tracker(meta.announce, meta.name, &peer_num); // 트래커 서버로 피어 주라고 요청하기
		
		for (int i=0; i<peer_num; i++){
			printf("seed_IP_arr[%d]: %s\n", i, seed_IP_arr[i]);
		}
		

		if (peer_num == 0){
			perror("다운받을 수 없습니다.");
			exit(1);
		}

		// 분배할 조각의 수와 피어의 수에 따라 인덱스 나누기
		int index_term = meta.piece_num / peer_num;
		int addition_term = meta.piece_num % peer_num;

		int start_index = 0;
		int end_index;
		
		// 저장공간 할당
		char temp_file_name[256];
		allocate_storage(meta, temp_file_name);

		// 스레드 함수의 매개 변수
		pthread_t threads[peer_num];
		down_request_targs args[peer_num];

		// 다운로드 진행바
		int downloaded_pieces = 0;
		pthread_mutex_t progress_mutex = PTHREAD_MUTEX_INITIALIZER;

		// 설정한 인덱스에 따라 다운로드 요청
		for (int i=0; i<peer_num; i++){
			args[i].meta_data = meta;

			strcpy(args[i].temp_file_name, temp_file_name);
			end_index = start_index + index_term -1;
			if (i<addition_term){
				end_index++;
			}

			if (end_index >= meta.piece_num) {
        		end_index = meta.piece_num - 1;
   			}

			strcpy(args[i].seed_IP, seed_IP_arr[i]);
			//printf("seedIP(main): %s", args.seed_IP); // debug
			args[i].start_index = start_index;
			args[i].end_index = end_index;

			args[i].mutex = &progress_mutex;
			args[i].downloaded_pieces = &downloaded_pieces;

			// 스레드 생성
			if (pthread_create(&threads[i], NULL, receive_piece, (void*)&args[i]) != 0) {
				perror("pthread_create");
				exit(1);
			}

			start_index = ++end_index;
		}

		for (int i = 0; i < peer_num; i++) {
			pthread_join(threads[i], NULL);
		}

		//다운로드 완료시 원래 파일 이름으로 변경
		if (rename(temp_file_name, meta.name) == 0) {
        printf("파일 이름이 성공적으로 변경되었습니다.\n");
		} else {
			perror("파일 이름 변경 실패");
			exit(1);
		}


		gettimeofday(&end, NULL);

		double diffTime = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);

		printf("다운로드 소요 시간 : %f sec\n", diffTime);

		if(report_to_tracker(meta.name)){
			printf("트래커 서버로 보고 완료\n");
		}else {
			printf("트래커 서버로 보고 실패\n");
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
