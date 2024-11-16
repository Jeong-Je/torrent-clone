#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 208000  // 송수신 버퍼 크기 (208KB)
#define PIECE_SIZE 131072   // 피스 크기 (128KB)
#define DELIMITER "gugubunja"  // 구분자 값
#define DELIMITER_SIZE 10      // 구분자 문자열 길이
#define OUTPUT_FILE "merged_file.dat"  // 병합된 파일 이름

// 데이터를 파일에 쓰는 함수
void write_piece_to_file(FILE *file, int piece_index, unsigned char *data, size_t data_size) {
    fseek(file, piece_index * PIECE_SIZE, SEEK_SET);  // 각 조각의 위치로 이동
    fwrite(data, 1, data_size, file);  // 데이터를 파일에 쓰기
    printf("Piece %d written to file (size: %zu bytes)\n", piece_index, data_size);
}

// 클라이언트로부터 데이터를 수신하는 함수
void handle_client(int client_sock) {
    FILE *output_file = fopen(OUTPUT_FILE, "wb+");
    if (output_file == NULL) {
        perror("Failed to open output file");
        close(client_sock);
        return;
    }

    unsigned char recv_buffer[BUFFER_SIZE];
    while (1) {
        ssize_t bytes_received = recv(client_sock, recv_buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("Receive failed");
            }
            break;
        }

        // 1. 피스 인덱스 추출
        int piece_index;
        memcpy(&piece_index, recv_buffer, sizeof(piece_index));

        // 2. 구분자 확인
        if (memcmp(recv_buffer + sizeof(piece_index), DELIMITER, DELIMITER_SIZE) != 0) {
            fprintf(stderr, "Invalid delimiter for piece %d\n", piece_index);
            continue;
        }

        // 3. 데이터 추출 및 파일 쓰기
        size_t data_size = bytes_received - sizeof(piece_index) - DELIMITER_SIZE;
        unsigned char *data = recv_buffer + sizeof(piece_index) + DELIMITER_SIZE;
        write_piece_to_file(output_file, piece_index, data, data_size);
    }

    fclose(output_file);
    close(client_sock);
}

// 서버 실행 함수
void start_server(const char *ip, int port) {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %s:%d\n", ip, port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");
        handle_client(client_sock);
    }

    close(server_sock);
}

int main() {
    const char *server_ip = "127.0.0.1";  // 서버 IP
    int server_port = 12345;  // 서버 포트
    start_server(server_ip, server_port);
    return 0;
}
