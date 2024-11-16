#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PIECE_SIZE 131072  // 128KB
#define BUFFER_SIZE 208000  // 208KB 송수신 버퍼 크기
#define DELIMITER 0xFF  // 구분자 (구분자가 될 수 있는 임의의 값)

void send_piece_with_index(int sockfd, const unsigned char *piece_data, size_t piece_size, int piece_index) {
    // 인덱스 + 구분자 + 피스 데이터를 담을 버퍼
    unsigned char buffer[BUFFER_SIZE];
    int offset = 0;

    // 인덱스를 먼저 넣기
    memcpy(buffer + offset, &piece_index, sizeof(piece_index));
    offset += sizeof(piece_index);

    // 구분자를 넣기
    buffer[offset] = DELIMITER;
    offset += 1;

    // 피스 데이터 넣기
    memcpy(buffer + offset, piece_data, piece_size);
    offset += piece_size;

    ssize_t bytes_sent = 0;
    while (bytes_sent < offset) {
        ssize_t sent = send(sockfd, buffer + bytes_sent, offset - bytes_sent, 0);
        if (sent < 0) {
            perror("Send failed");
            break;
        }
        bytes_sent += sent;
    }
    printf("Piece %d sent: %ld bytes\n", piece_index, bytes_sent);
}

void send_file_with_index(const char *ip, int port, const char *filename) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File open failed");
        close(sockfd);
        return;
    }

    unsigned char buffer[PIECE_SIZE];
    int piece_index = 0;
    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, PIECE_SIZE, file);
        send_piece_with_index(sockfd, buffer, bytes_read, piece_index++);
    }

    fclose(file);
    close(sockfd);
}

int main() {
    const char *ip = "127.0.0.1";
    int port = 12345;
    const char *filename = "sample_file.txt";

    send_file_with_index(ip, port, filename);
    return 0;
}
