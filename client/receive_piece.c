#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "env.h"
#include "meta.h"
#include "receive_piece.h"


void* receive_piece(void* vargs){
        printf("receive_piece 실행\n");
        // 스레드 매개변수 할당
        down_request_targs* args = (down_request_targs*)vargs;
        meta meta_data = args->meta_data;
        int start_index = args->start_index;
        int end_index = args->end_index;
        char seed_IP[16];
        strcpy(seed_IP, args->seed_IP);
        char temp_file_name[256];
        // strcpy(temp_file_name, args->temp_file_name);
        int fd = args->fd;
        
        printf("startindex: %d\n", start_index);
        printf("endindex: %d\n", end_index);

        // 파일 수신할 버퍼 및 변수
        int sd;
        int header_size = atoi(get_env("PIECE_HEADER_SIZE"));
        int payload_size = meta_data.piece_length;
        char header_buf[header_size];
        char payload_buf[payload_size];
        int64_t received_piece_index;
        int64_t received_piece_size;
        
        // 피스 파일 요청 구조체
        file_request req;
        req.meta_data = meta_data;
        req.start_chunk = start_index;
        req.end_chunk = end_index;

        // tcp 통신
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
        // printf("seed_IP: %s\n", seed_IP);       // test
        sin.sin_addr.s_addr = inet_addr(seed_IP);

        printf("connect 전\n");
        if (connect(sd, (struct sockaddr *)&sin, sizeof(sin))){
                perror("connect");
                exit(1);
        }
        printf("connect 후\n");

        // 할당한 저장공간 열기
        // int fd = open(temp_file_name, O_WRONLY, 0644);
        // if (fd == -1){
        //         perror("임시 파일 열기 실패");
        //         exit(1);
        // }

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
                // printf("piece index: %d\n", received_piece_index);
                // printf("piece size: %d\n", received_piece_size);
                
                // 피스 파일 수신
                bytes_received = recv(sd, payload_buf, received_piece_size, MSG_WAITALL);
                // printf("received piece: %s\n", payload_buf);
                if (bytes_received <= 0) break;

                // 할당한 저장공간에 수신한 피스 배치
                lseek(fd, i * meta_data.piece_length, SEEK_SET);
                write(fd, payload_buf, received_piece_size);
                // printf("피스 배치 완료\n");
        }

        close(sd);
        // close(fd);

        
        pthread_exit(NULL);
}