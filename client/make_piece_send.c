#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PIECE_SIZE 131072  // 피스 사이즈 (128KB)
#define BUFFER_SIZE 208000  // 송수신 버퍼 208KB 고정
#define DELIMITER "space"  // 구분자 값
#define DELIMITER_SIZE 5   // 구분자 문자열의 길이

// 피스를 전송하는 함수
void send_piece(int sockfd, int piece_index, unsigned char *buffer, size_t bytes_read) {
    unsigned char send_buffer[BUFFER_SIZE];
    int offset = 0;

    // 1. 피스 인덱스를 보내기
    memcpy(send_buffer + offset, &piece_index, sizeof(piece_index));
    offset += sizeof(piece_index);

    // 2. 구분자를 보내기
    memcpy(send_buffer + offset, DELIMITER, DELIMITER_SIZE);
    offset += DELIMITER_SIZE;

    // 3. 피스 데이터를 보내기
    memcpy(send_buffer + offset, buffer, bytes_read);
    offset += bytes_read;

    // 4. 데이터를 송신합니다.
    ssize_t bytes_sent = 0;
    while (bytes_sent < offset) {
        ssize_t sent = send(sockfd, send_buffer + bytes_sent, offset - bytes_sent, 0);
        if (sent < 0) {
            perror("Send failed");
            break;
        }
        bytes_sent += sent;
    }
    printf("Piece %d sent: %ld bytes\n", piece_index, bytes_sent);
}

// 파일을 128KB 피스로 나누어 전송하는 함수
void split_file_and_send(const char *filename, const char *server_ip, int server_port, int start_index, int end_index) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int num_pieces = (file_size + PIECE_SIZE - 1) / PIECE_SIZE;  // 피스의 개수 계산

    printf("File size: %ld bytes\n", file_size);
    printf("Total pieces: %d\n", num_pieces);

    // 소켓 연결 설정
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        fclose(file);
        return;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sockfd);
        fclose(file);
        return;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        fclose(file);
        return;
    }

    unsigned char buffer[PIECE_SIZE];
    for (int i = start_index; i < end_index; i++) {
        fseek(file, i * PIECE_SIZE, SEEK_SET);
        size_t bytes_read = fread(buffer, 1, PIECE_SIZE, file);
        printf("Reading piece %d/%d (size: %zu bytes)\n", i + 1, num_pieces, bytes_read);

        // 피스 데이터를 네트워크로 전송
        send_piece(sockfd, i, buffer, bytes_read);

        // 추가적으로 수신 대기 및 재전송 처리 추가 가능
    }

    // 전송 완료 후 소켓 닫기
    close(sockfd);
    fclose(file);
}

int main() {
    const char *filename = "example_file.dat";  // 전송할 파일 경로
    const char *server_ip = "127.0.0.1";  // 서버 IP
    int server_port = 12345;  // 서버 포트

    // 예시: 첫 번째 클라이언트는 피스 0~4까지, 두 번째 클라이언트는 피스 5~9까지 전송
    int start_index = 0;
    int end_index = 4;
    split_file_and_send(filename, server_ip, server_port, start_index, end_index);

    return 0;
}
