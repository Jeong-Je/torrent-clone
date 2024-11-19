#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "meta.h"
#include "download_piece.h"

#define PORTNUM 20913
#define BUF_SIZE 131077 // 4 + 1 + 16348

void download_piece(meta meta, char* temp_file_name){
    unsigned char buf[BUF_SIZE];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    // 포트넘버 재사용 설정 코드(임시)
    int opt = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(sd);
        exit(1);
    }

    memset((char*)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("0.0.0.0");

    if (bind(sd, (struct sockaddr*)&sin, sizeof(sin))){
        perror("bind");
        exit(1);
    }

    if(listen(sd, 5)){
        perror("listen");
        exit(1);
    }
 
    if((ns = accept(sd, (struct sockaddr*)&cli, &clientlen)) == -1){
        perror("accept");
        exit(1);
    }

    // recv
    ssize_t total_bytes = 0;
    while (total_bytes < BUF_SIZE) {
        ssize_t bytes_received = recv(ns, buf + total_bytes, BUF_SIZE - total_bytes, 0);
        if (bytes_received == -1) {
            perror("recv failed");
            close(ns);
            return;
        } else if (bytes_received == 0) {
            printf("Connection closed by peer\n");
            break;
        }
        total_bytes += bytes_received;

        if (total_bytes >= 131077) {
            break;
        }
    }

    printf("Total bytes received: %ld\n", total_bytes);

    printf("received data(일부): ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");

    char *separator = memchr(buf, 0xFF, total_bytes);
    if (separator == NULL){
        printf("구분자를 찾지 못하였습니다.\n");
    } else printf("구분자: %02X\n", (unsigned char)*separator);
    printf("buf 시작위치: %p\n", buf);
    printf("구분자 위치: %p\n", separator);

    
    // 피스 인덱스 추출
    int piece_index;
    memcpy(&piece_index, buf, sizeof(int));

    // 피스 파일 추출
    char *piece_file = separator + 1; // 구분자 바로 다음부터 시작

    printf("Piece Index: %d\n", piece_index);
    //printf("Piece File Content: %s\n", piece_file);
    
        
    // 할당한 저장공간에 수신한 피스 배치
    int fd = open(temp_file_name, O_WRONLY, 0644);
    if (fd == -1){
        perror("임시 파일 열기 실패");
        exit(1);
    }
    lseek(fd, piece_index * meta.piece_length, SEEK_SET);
    write(fd, piece_file, total_bytes - (separator - (char *)buf) - 1);
    printf("피스 배치 완료\n");

    
    close(ns);
    close(sd);
}