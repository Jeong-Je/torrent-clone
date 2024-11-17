#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define CHUNK_SIZE 131072  // 128KB

// 파일을 128KB 조각으로 나누어 전송
void send_file(FILE *file, int socket) {
    char buffer[CHUNK_SIZE];
    size_t bytes_read;
    int index = 0;  // 각 조각의 인덱스 값

    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        // 조각 전송 전에 인덱스 값 전송
        if (send(socket, &index, sizeof(index), 0) == -1) {
            perror("Failed to send index");
            exit(EXIT_FAILURE);
        }

        // 조각 데이터 전송
        if (send(socket, buffer, bytes_read, 0) == -1) {
            perror("Failed to send chunk");
            exit(EXIT_FAILURE);
        }

        printf("Sent chunk with index: %d\n", index);
        index++;  // 인덱스 값 증가
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // 파일 열기
    FILE *file = fopen("random_512kb.txt", "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 바인드
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 리슨
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for connection...\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection established. Sending file...\n");
    send_file(file, new_socket);
    printf("File sent successfully.\n");

    fclose(file);
    close(new_socket);
    close(server_fd);

    return 0;
}
