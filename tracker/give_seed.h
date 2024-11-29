#ifndef GIVE_SEED_H
#define GIVE_SEED_H

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>
#include <sys/socket.h>  // recv 함수 정의
#include <arpa/inet.h>   // sockaddr_in, htons 등
#include <unistd.h>      // close()

#include "env.h"

int give_seed(char* file_name, int new_socket);

#endif