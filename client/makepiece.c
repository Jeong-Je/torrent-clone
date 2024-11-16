#include <stdio.h>
#include <stdlib.h>

#define PIECE_SIZE 131072  // 피스 사이즈
#define BUFFER_SIZE 208000  // 송수신 버퍼 208KB 고정

// 파일을 128KB 피스로 나누는 함수
void split_file(const char *filename) {
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

    unsigned char buffer[PIECE_SIZE];
    for (int i = 0; i < num_pieces; i++) {
        size_t bytes_read = fread(buffer, 1, PIECE_SIZE, file);
        printf("Reading piece %d/%d (size: %zu bytes)\n", i + 1, num_pieces, bytes_read);

        // 여기서 buffer에 있는 데이터를 네트워크로 전송하거나 다른 처리를 할 수 있습니다
        // 예를 들어 send_piece(i, buffer, bytes_read);
    }

    fclose(file);
}

int main() {
    const char *filename = "sample_file.txt";
    split_file(filename);
    return 0;
}
