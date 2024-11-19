#ifndef MAKE_SERVER_H
#define MAKE_SERVER_H

#define PORTNUM 12344
#define CLIENT_MAX 10

#include <stdio.h>      // printf, perror 등의 표준 입출력 함수
#include <stdlib.h>     // exit 함수
#include <string.h>     // 문자열 관련 함수 (memset, memcpy 등)
#include <unistd.h>     // close, fork 등의 POSIX 함수
#include <arpa/inet.h>  // 주소 변환 함수 (inet_ntoa, htons 등)
#include <sys/types.h>  // 다양한 데이터 타입, pid_t 포함
#include <sys/socket.h> // 소켓 관련 함수 및 구조체
#include <netinet/in.h> // sockaddr_in 구조체

int init_server();

#endif
