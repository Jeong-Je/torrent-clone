#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "report_to_tracker.h"


bool report_to_tracker(char * torrent_file){
    char* port_num_str = get_env("SERVER_PORT");
    int port_num = atoi(port_num_str);
    free(port_num_str); // 메모리 반환

    char* server_ip = get_env("SERVER_IP");

    int sd;
    char buffer[256];

    sprintf(buffer, "iam_seed:%s", torrent_file);

    struct sockaddr_in sin;

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port_num);
    sin.sin_addr.s_addr = inet_addr(server_ip);
    free(server_ip);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        return false; 
    }
    
    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        return false;
    }

    if(send(sd, buffer, strlen(buffer)+1, 0) == -1) {
        perror("send");
        return false;
    }

    close(sd);

    return true;
}