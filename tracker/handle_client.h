#ifndef HANDEL_CLIENT_H
#define HANDEL_CLIENT_H

#define MAX_BUF 1024

#include <stdio.h>      // printf, perror 등의 표준 입출력 함수
#include <stdlib.h>     // exit 함수
#include <string.h>     // strcmp, memset 등의 문자열 처리 함수
#include <unistd.h>     // fork, close 등의 POSIX 함수
#include <sys/types.h>  // 다양한 데이터 타입, pid_t 포함
#include <sys/socket.h> // 소켓 프로그래밍 관련 함수 (socket, bind, recv 등)
#include <arpa/inet.h>  // 주소 변환 (inet_ntoa, htons 등)
#include <netinet/in.h> // sockaddr_in 구조체 (IP 소켓 주소 구조체)
#include <sys/wait.h>

int handle_client(int* new_socket, struct sockaddr_in* client_address);

int recv_buf(int* new_socket, char* buf);

#endif
