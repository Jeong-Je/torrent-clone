#include <stdio.h>
#include <stdlib.h>
#include <libgen.h> // basename 함수를 위해서 사용
#include <sys/stat.h>

#include "create_torrent.h"

void create_torrent(char *file_path, char *tracker_url) {
        struct stat file_stat; // stat 구조체 선언

        if(stat(file_path, &file_stat) == -1){
                perror("파일 정보 조회 실패");
                exit(1);
        }

        // torrent 파일 open(생성)
        char torrent_file[FILE_SIZE];
        sprintf(torrent_file,"%s.torrent", file_path);
        FILE *fp = fopen(torrent_file, "wb");

        // file_path에서 filename만 추출
        char* filename = basename(file_path);

        // 메타데이터 생성 시작
        bencode_dict_start(fp);

        // announce (tracker URL);
        bencode_string(fp, "announce");
        bencode_string(fp, tracker_url);

        // info 딕셔너리 시작
        bencode_dict_start(fp);

        // name (파일 이름)
        bencode_string(fp, "name");
        bencode_string(fp, filename);

        // file_length (원본 파일의 크기)
        bencode_string(fp, "file_length");
        bencode_int(fp, file_stat.st_size);

        // piece_length (조각 크기)
        bencode_string(fp, "piece_length");
        bencode_int(fp, PIECE_LENGTH);

        // info 딕셔너리 종료
        bencode_dict_end(fp);

        // 메타데이터 생성 종료
        bencode_dict_end(fp);

        // fclose
        fclose(fp);


        printf("파일 이름:%s\n", filename);
        printf("파일 크기:%ld 바이트\n", file_stat.st_size);
        printf("%s 파일을 정상적으로 생성하였습니다.\n", torrent_file);
}
