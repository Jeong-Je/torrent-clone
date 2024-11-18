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

void send_file_with_index_and_size(int client_socket, const char* filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("파일 열기 실패");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // 파일 크기 확인
    struct stat file_stat;
    if (stat(filename, &file_stat) < 0) {
        perror("파일 크기 확인 실패");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    int64_t file_size = file_stat.st_size;

    // 파일 크기 전송
    if (send(client_socket, &file_size, sizeof(file_size), 0) < 0) {
        perror("파일 크기 전송 실패");
        close(file_fd);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // 조각 데이터 전송
    char buffer[CHUNK_SIZE];
    ssize_t bytes_read;
    int64_t chunk_index = 0;

    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        // 인덱스 전송
        if (send(client_socket, &chunk_index, sizeof(chunk_index), 0) < 0) {
            perror("인덱스 전송 실패");
            close(file_fd);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // 조각 크기 전송
        int64_t chunk_size = bytes_read;
        if (send(client_socket, &chunk_size, sizeof(chunk_size), 0) < 0) {
            perror("조각 크기 전송 실패");
            close(file_fd);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // 데이터 전송
        if (send(client_socket, buffer, chunk_size, 0) < 0) {
            perror("파일 조각 전송 실패");
            close(file_fd);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        printf("전송한 조각: 인덱스=%lld, 크기=%ld 바이트\n", chunk_index, bytes_read);
        chunk_index++;
    }

    // 전송 완료 표시
    int64_t end_marker = -1;
    send(client_socket, &end_marker, sizeof(end_marker), 0);

    printf("파일 전송 완료: %s\n", filename);
    close(file_fd);
}

int main() {
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

    printf("서버가 파일 전송 대기 중입니다...\n");

    client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket < 0) {
        perror("클라이언트 연결 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send_file_with_index_and_size(client_socket, "test.txt");
    close(client_socket);
    close(server_fd);

    return 0;
}
