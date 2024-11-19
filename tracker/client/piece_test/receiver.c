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

void receive_file_with_index_and_size(int server_socket, const char* output_filename) {
    int file_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd < 0) {
        perror("파일 열기 실패");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 파일 크기 수신
    int64_t expected_size;
    if (recv(server_socket, &expected_size, sizeof(expected_size), 0) <= 0) {
        perror("파일 크기 수신 실패");
        close(file_fd);
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    printf("받을 파일 크기: %lld 바이트\n", expected_size);

    // 조각 수신
    char buffer[CHUNK_SIZE];
    int64_t chunk_index;
    int64_t chunk_size;
    int64_t total_received = 0;

    while (1) {
        // 인덱스 수신
        if (recv(server_socket, &chunk_index, sizeof(chunk_index), 0) <= 0) {
            perror("인덱스 수신 실패");
            break;
        }

        // 전송 완료 확인
        if (chunk_index == -1) {
            printf("파일 수신 완료: %s\n", output_filename);
            break;
        }

        // 조각 크기 수신
        if (recv(server_socket, &chunk_size, sizeof(chunk_size), 0) <= 0) {
            perror("조각 크기 수신 실패");
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

        printf("받은 조각: 인덱스=%lld, 크기=%ld 바이트\n", chunk_index, bytes_received);
        total_received += bytes_received;
    }

    close(file_fd);

    // 파일 크기 검증
    if (total_received != expected_size) {
        fprintf(stderr, "경고: 수신된 파일 크기 불일치! (%lld != %lld)\n", total_received, expected_size);
    } else {
        printf("파일 크기 검증 완료: %lld 바이트\n", total_received);
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

    receive_file_with_index_and_size(server_socket, "received_test.txt");
    close(server_socket);
    return 0;
}
