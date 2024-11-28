#ifndef UPDATE_SEEDLIST_H
#define UPDATE_SEEDLIST_H

#include<stdio.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "request.h"
#include <sys/socket.h>  // recv 함수 정의
#include <arpa/inet.h>   // sockaddr_in, htons 등
#include <unistd.h>      // close()

int update_seedlist(Request req);

#endif
