#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "env.h"

#define PORT 8080
#define CHUNK_SIZE 131072 // 128KB
#define HEADER_SIZE 16    // 인덱스(8바이트) + 조각 크기(8바이트)

typedef struct {
    char filename[256];
    int64_t start_chunk;
    int64_t end_chunk;
} file_request;

void send_pieces() {
    printf("서버 리스닝 상태로 대기 중...\n");

    char* port = get_env("SERVER_PORT");
    char* header_size_str = get_env("PIECE_HEADER_SIZE");
    char* piece_size_str = get_env("PIECE_LENGTH");

    int header_size = atoi(header_size_str); // char -> int 
    int piece_size = atoi(piece_size_str); // char -> int

    int server_fd, client_socket;
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
    server_addr.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 대기
    if (listen(server_fd, 3) < 0) {
        perror("리스닝 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("클라이언트 연결 실패");
            continue;
        }

        // 클라이언트로부터 파일명, 시작 인덱스, 끝 인덱스를 한 번에 수신
        file_request request;
        if (recv(client_socket, &request, sizeof(request), 0) <= 0) {
            perror("클라이언트 요청 수신 실패");
            close(client_socket);
            continue;
        }

        printf("수신된 요청 - 파일명: %s, 시작 인덱스: %lld, 끝 인덱스: %lld\n", request.filename, request.start_chunk, request.end_chunk);

        // 요청받은 파일 조각 전송
        int file_fd = open(request.filename, O_RDONLY);
        if (file_fd < 0) {
            perror("파일 열기 실패");
            close(client_socket);
            continue;
        }

        char buffer[piece_size + header_size];
        ssize_t bytes_read;
        int64_t chunk_index = 0;

        // 파일 조각 전송
        while ((bytes_read = read(file_fd, buffer + header_size, piece_size)) > 0) {
            if (chunk_index >= request.start_chunk && chunk_index <= request.end_chunk) {
                memcpy(buffer, &chunk_index, sizeof(chunk_index));                  // 인덱스
                memcpy(buffer + sizeof(chunk_index), &bytes_read, sizeof(bytes_read)); // 크기

                if (send(client_socket, buffer, header_size + bytes_read, 0) < 0) {
                    perror("조각 전송 실패");
                    close(file_fd);
                    close(client_socket);
                    break;
                }
                printf("송신: 조각 %lld (%ld 바이트)\n", chunk_index, bytes_read);
            }
            if (chunk_index > request.end_chunk) break;
            chunk_index++;
        }

        close(file_fd);
        close(client_socket);
        printf("송신 완료: 조각 %lld~%lld\n", request.start_chunk, request.end_chunk);
    }

    close(server_fd);
}
