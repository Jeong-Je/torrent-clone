#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h> // int64_t 사용을 위해 추가
#include <arpa/inet.h>
#include <fcntl.h>

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

    receive_file_in_chunks(server_socket, "received_test.txt"); // 저장할 파일 이름
    close(server_socket);

    return 0;
}
