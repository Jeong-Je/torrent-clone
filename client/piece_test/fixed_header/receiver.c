#include <pthread.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CHUNK_SIZE 131072
#define HEADER_SIZE 16
#define TOTAL_CHUNKS 4  // 총 조각 개수

typedef struct {
    int socket_fd;
    char* file_data;
    int* received_flags;
} ThreadData;

void* receive_chunks(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char buffer[CHUNK_SIZE + HEADER_SIZE];
    int64_t chunk_index;
    int64_t chunk_size;

    while (1) {
        ssize_t bytes_received = recv(data->socket_fd, buffer, HEADER_SIZE, MSG_WAITALL);
        if (bytes_received <= 0) break;

        memcpy(&chunk_index, buffer, sizeof(chunk_index));
        memcpy(&chunk_size, buffer + sizeof(chunk_index), sizeof(chunk_size));

        bytes_received = recv(data->socket_fd, buffer, chunk_size, MSG_WAITALL);
        if (bytes_received <= 0) break;

        memcpy(data->file_data + chunk_index * CHUNK_SIZE, buffer, bytes_received);
        data->received_flags[chunk_index] = 1;

        printf("수신: 조각 %ld (%ld 바이트)\n", chunk_index, bytes_received);
    }

    close(data->socket_fd);
    return NULL;
}

int main() {
    const int num_senders = 1; // 송신자 수
    pthread_t threads[num_senders];
    ThreadData thread_data[num_senders];

    char* file_data = calloc(TOTAL_CHUNKS, CHUNK_SIZE);
    int* received_flags = calloc(TOTAL_CHUNKS, sizeof(int));

    for (int i = 0; i < num_senders; i++) {
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd < 0) {
            perror("소켓 생성 실패");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080 + i); // 송신자별 포트
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("서버 연결 실패");
            close(sock_fd);
            exit(EXIT_FAILURE);
        }

        thread_data[i].socket_fd = sock_fd;
        thread_data[i].file_data = file_data;
        thread_data[i].received_flags = received_flags;

        pthread_create(&threads[i], NULL, receive_chunks, &thread_data[i]);
    }

    for (int i = 0; i < num_senders; i++) {
        pthread_join(threads[i], NULL);
    }

    // 병합
    int file_fd = open("received_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    for (int64_t i = 0; i < TOTAL_CHUNKS; i++) {
        if (!received_flags[i]) {
            printf("누락된 조각: %ld\n", i);
            continue;
        }
        write(file_fd, file_data + i * CHUNK_SIZE, CHUNK_SIZE);
    }

    close(file_fd);
    free(file_data);
    free(received_flags);

    printf("파일 병합 완료: received_file.txt\n");
    return 0;
}
