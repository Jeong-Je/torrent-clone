#ifndef REQUEST_THREAD_H
#define REQUEST_THREAD_H

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<pthread.h>
#include <sys/socket.h>  // recv 함수 정의
#include <arpa/inet.h>   // sockaddr_in, htons 등
#include <unistd.h>      // close()

void* request_thread(void* data);

#endif
