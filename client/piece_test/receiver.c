#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 131072  // 128KB

// 파일 수신 및 인덱스 값을 기준으로 정렬
void receive_file(int socket) {
    FILE *file = fopen("received_file.txt", "wb");
    if (file == NULL) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    int index;
    int chunk_count = 0;

    // 파일 크기나 조각 수를 알 수 없으므로 무한 루프를 돌며 수신
    while (1) {
        // 인덱스 값 수신
        if (recv(socket, &index, sizeof(index), 0) <= 0) {
            break;  // 연결 종료
        }

        // 데이터 수신
        bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }

        printf("Received chunk with index: %d\n", index);

        // 받은 데이터 저장 (순서가 맞지 않더라도 임시 파일에 저장)
        fseek(file, index * BUFFER_SIZE, SEEK_SET);
        fwrite(buffer, 1, bytes_received, file);

        chunk_count++;
    }

    fclose(file);
    printf("File received successfully with %d chunks.\n", chunk_count);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Receiving file...\n");
    receive_file(sock);
    close(sock);

    return 0;
}
