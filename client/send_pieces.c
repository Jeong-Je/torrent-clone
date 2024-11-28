#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "env.h"
#include "receive_piece.h"
#include "meta.h"

// 스레드 함수 매개변수 구조체
typedef struct {
    int client_socket;
    meta meta_data;
    int64_t start_chunk;
    int64_t end_chunk;
} thread_args;

// 클라이언트 요청을 처리하는 함수
void* handle_client(void* varg) {
    thread_args* args = (thread_args*)varg;

    int header_size = atoi(get_env("PIECE_HEADER_SIZE"));
    int piece_size = args->meta_data.piece_length;
    
    char buffer[piece_size + header_size];
    ssize_t bytes_read;
    int64_t chunk_index = 0;

    // 파일 열기
    int file_fd = open(args->meta_data.name, O_RDONLY);
    if (file_fd < 0) {
        perror("파일 열기 실패");
        close(args->client_socket);
        free(args); // 요청 메모리 해제
        pthread_exit(NULL);
    }

    // 파일 조각 전송
    while ((bytes_read = read(file_fd, buffer + header_size, piece_size)) > 0) {
        if (chunk_index >= args->start_chunk && chunk_index <= args->end_chunk) {
            memcpy(buffer, &chunk_index, sizeof(chunk_index));                  // 인덱스
            memcpy(buffer + sizeof(chunk_index), &bytes_read, sizeof(bytes_read)); // 크기

            if (send(args->client_socket, buffer, header_size + bytes_read, 0) < 0) {
                perror("조각 전송 실패");
                close(file_fd);
                close(args->client_socket);
                free(args); // 요청 메모리 해제
                pthread_exit(NULL);
            }
            printf("송신: 조각 %lld (%ld 바이트)\n", chunk_index, bytes_read);
        }
        if (chunk_index > args->end_chunk) break;
        chunk_index++;
    }

    close(file_fd);
    close(args->client_socket);
    printf("송신 완료: 조각 %lld~%lld\n", args->start_chunk, args->end_chunk);
    free(args); // 요청 메모리 해제
    pthread_exit(NULL);
}

void send_pieces() {
    printf("서버 리스닝 상태로 대기 중...\n");

    char* port_str = get_env("SERVER_PORT");
    char* header_size_str = get_env("PIECE_HEADER_SIZE");

    int port = atoi(port_str);

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 서버 소켓 생성
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 바인딩
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 대기
    if (listen(server_fd, 10) < 0) { // 최대 10개의 연결 대기
        perror("리스닝 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 클라이언트 연결 수락
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("클라이언트 연결 실패");
            continue;
        }

        // 클라이언트로부터 파일명, 시작 인덱스, 끝 인덱스를 한 번에 수신
        file_request* request = (file_request*)malloc(sizeof(file_request));
        if (recv(client_socket, request, sizeof(file_request), 0) <= 0) {
            perror("클라이언트 요청 수신 실패");
            close(client_socket);
            free(request); // 요청 메모리 해제
            continue;
        }

        printf("수신된 요청 - 파일명: %s, 시작 인덱스: %lld, 끝 인덱스: %lld\n", request->meta_data.name, request->start_chunk, request->end_chunk);

        // 스레드 함수 매개변수 설정
        thread_args* args = (thread_args*)malloc(sizeof(thread_args));
        args->client_socket = client_socket;
        args->meta_data = request->meta_data;
        args->start_chunk = request->start_chunk;
        args->end_chunk = request->end_chunk;
        

        // 새로운 스레드 생성하여 클라이언트 요청 처리
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void*)args) != 0) {
            perror("스레드 생성 실패");
            close(client_socket);
            free(request); // 요청 메모리 해제
        }

        // 스레드를 분리하여 종료를 기다리지 않음
        pthread_detach(thread_id);
    }

    close(server_fd);
}