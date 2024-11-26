#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"
#include "meta.h"

// 피스 파일 요청 구조체
typedef struct {
    char filename[256];
    int64_t start_chunk;
    int64_t end_chunk;
} file_request;

void receive_piece(meta meta, int start_index, int end_index, in_addr_t seed_IP, char* temp_file_name){
        int sd;
        int header_size = atoi(get_env("PIECE_HEADER_SIZE"));
        int payload_size = atoi(get_env("PIECE_LENGTH"));
        char header_buf[header_size];
        char payload_buf[payload_size];
        int64_t received_piece_index;
        int64_t received_piece_size;
        

        file_request req;
        strcpy(req.filename, meta.name);
        req.start_chunk = start_index;
        req.end_chunk = end_index;

        char* server_port = get_env("SERVER_PORT");
        int port = atoi(server_port);
        free(server_port);
        struct sockaddr_in sin;

        if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
                perror("socket");
                exit(1);
        }

        memset((char *)&sin, '\0', sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = seed_IP;

        if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))){
                perror("connect");
                exit(1);
        }

        // 할당한 저장공간 열기
        int fd = open(temp_file_name, O_WRONLY, 0644);
        if (fd == -1){
                perror("임시 파일 열기 실패");
                exit(1);
        }

        // 피스 파일 요청 구조체 송신
        if (send(sd, &req, sizeof(req), 0) == -1){
                perror("piece struct send");
                exit(1);
        }

        // 파일 받기
        for (int i=start_index; i<=end_index; i++){
                // 헤더 수신
                ssize_t bytes_received = recv(sd, header_buf, header_size, MSG_WAITALL);
                if (bytes_received <= 0) break;

                memcpy(&received_piece_index, header_buf, sizeof(received_piece_index));
                memcpy(&received_piece_size, header_buf + sizeof(received_piece_index), sizeof(received_piece_size));
                printf("piece index: %d\n", received_piece_index);
                printf("piece size: %d\n", received_piece_size);
                
                // 피스 파일 수신
                bytes_received = recv(sd, payload_buf, received_piece_size, MSG_WAITALL);
                printf("received piece: %s\n", payload_buf);
                if (bytes_received <= 0) break;

                // 할당한 저장공간에 수신한 피스 배치
                lseek(fd, start_index * meta.piece_length, SEEK_SET);
                write(fd, payload_buf, received_piece_size);
                printf("피스 배치 완료\n");
        }

        close(sd);

}