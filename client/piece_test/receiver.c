#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>

#define PORT 8080
#define CHUNK_SIZE 131072 // 128KB

void receive_file_in_chunks(int server_socket, const char* output_filename) {
    int file_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        perror("파일 열기 실패");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    char buffer[CHUNK_SIZE];
    int64_t chunk_size;

    while (1) {
        // 조각 크기 수신
        ssize_t size_received = recv(server_socket, &chunk_size, sizeof(chunk_size), 0);
        if (size_received <= 0) {
            perror("조각 크기 수신 실패");
            break;
        }

        // 크기가 0이면 전송 완료
        if (chunk_size == 0) {
            printf("파일 수신 완료: %s\n", output_filename);
            break;
        }

        // 데이터 수신
        ssize_t bytes_received = recv(server_socket, buffer, chunk_size, MSG_WAITALL);
        if (bytes_received <= 0) {
            perror("파일 조각 수신 실패");
            break;
        }

        if (write(file_fd, buffer, bytes_received) < 0) {
            perror("파일 쓰기 실패");
            close(file_fd);
            close(server_socket);
            exit(EXIT_FAILURE);
        }

        printf("받은 조각 크기: %ld 바이트\n", bytes_received);
    }

    close(file_fd);
}

void receive_file_with_size(int server_socket, const char* output_filename) {
    int64_t expected_size;

    // 파일 크기 수신
    if (recv(server_socket, &expected_size, sizeof(expected_size), 0) <= 0) {
        perror("파일 크기 수신 실패");
        exit(EXIT_FAILURE);
    }
    printf("받을 파일 크기: %lld 바이트\n", expected_size);

    // 파일 조각 수신
    receive_file_in_chunks(server_socket, output_filename);

    // 받은 파일 크기 확인
    struct stat file_stat;
    if (stat(output_filename, &file_stat) < 0) {
        perror("수신 파일 크기 확인 실패");
        exit(EXIT_FAILURE);
    }

    if (file_stat.st_size != expected_size) {
        fprintf(stderr, "경고: 수신된 파일 크기 불일치! (%lld != %lld)\n", file_stat.st_size, expected_size);
    } else {
        printf("파일 크기 일치: %lld 바이트\n", file_stat.st_size);
    }
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("IP 주소 변환 실패");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("서버 연결 실패");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    receive_file_with_size(server_socket, "received_test.txt"); // 저장할 파일 이름
    close(server_socket);

    return 0;
}
