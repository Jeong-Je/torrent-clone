#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "meta.h"
#include "download_piece.h"

#define PORTNUM 20913
#define BUF_SIZE 16348

int download_piece(meta meta, char* temp_file_name){
    char buf[BUF_SIZE];
    struct sockaddr_in sin, cli;
    int sd, ns, clientlen = sizeof(cli);

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
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

    // 피스 인덱스와 같은 수신 구조 및 피스 수신 추후 구현
    if (recv(ns, buf, sizeof(buf)+1, 0) == -1){
        perror("recv");
        exit(1);
    }
        

    // 할당한 저장공간에 수신한 피스 배치
    int fd = open(temp_file_name, O_WRONLY, 0644);
    // lseek(fd, piece_index * meta.piece_length,SEEK_SET);
    // write(fd, buf, strlen(buf));


    
    close(ns);
    close(sd);

    return 0;
}