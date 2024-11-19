#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define PORT 8080
#define CHUNK_SIZE 131072 // 128KB
#define HEADER_SIZE 16    // 인덱스(8바이트) + 조각 크기(8바이트)

void send_specific_chunks(int client_socket, const char* filename, int64_t start_chunk, int64_t end_chunk) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("파일 열기 실패");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    char buffer[CHUNK_SIZE + HEADER_SIZE];
    ssize_t bytes_read;
    int64_t chunk_index = 0;

    while ((bytes_read = read(file_fd, buffer + HEADER_SIZE, CHUNK_SIZE)) > 0) {
        if (chunk_index >= start_chunk && chunk_index <= end_chunk) {
            memcpy(buffer, &chunk_index, sizeof(chunk_index));                  // 인덱스
            memcpy(buffer + sizeof(chunk_index), &bytes_read, sizeof(bytes_read)); // 크기

            if (send(client_socket, buffer, HEADER_SIZE + bytes_read, 0) < 0) {
                perror("조각 전송 실패");
                close(file_fd);
                close(client_socket);
                exit(EXIT_FAILURE);
            }
            printf("송신: 조각 %ld (%ld 바이트)\n", chunk_index, bytes_read);
        }
        if (chunk_index > end_chunk) break;
        chunk_index++;
    }

    close(file_fd);
    close(client_socket);
    printf("송신 완료: 조각 %ld~%ld\n", start_chunk, end_chunk);
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "사용법: %s <파일이름> <시작조각> <끝조각>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    int64_t start_chunk = atoll(argv[2]);
    int64_t end_chunk = atoll(argv[3]);

    int server_fd, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("리스닝 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("서버가 조각 전송 대기 중...\n");
    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket < 0) {
        perror("클라이언트 연결 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send_specific_chunks(client_socket, filename, start_chunk, end_chunk);
    close(server_fd);
    return 0;
}
