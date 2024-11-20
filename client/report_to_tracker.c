#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "report_to_tracker.h"

#define PORTNUM 12344


bool report_to_tracker(char * torrent_file){
    int sd;
    char buffer[256];

    sprintf(buffer, "iam_seed:%s", torrent_file);

    struct sockaddr_in sin;

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("113.198.138.212");

    printf("1\n");
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        return false; 
    }
    printf("2\n");
    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        return false;
    }
    printf("3\n");
    if(send(sd, buffer, strlen(buffer)+1, 0) == -1) {
        perror("send");
        return false;
    }
    
    printf("4\n");

    close(sd);

    return true;
}