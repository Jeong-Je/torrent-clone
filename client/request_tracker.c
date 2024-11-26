#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


void request_tracker(char *announce, char *file_name){
    char *ip = strtok(announce, ":"); // ":" 문자를 기준으로 앞 문자열 (ip 주소)
    char *port = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열 (port 번호)

    int sd;
    char buffer[256];
    struct sockaddr_in sin;

    sprintf(buffer, "give_seed:%s.torrent", file_name);

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(atoi(port));

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1); 
    }
    
    if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("connect");
        exit(1);
    }

    if(send(sd, buffer, strlen(buffer)+1, 0) == -1) {
        perror("send");
        exit(1);
    }

    if(recv(sd, buffer, sizeof(buffer), 0) == 0) {
        perror("recv");
        exit(1);
    }

    printf("recv buffer: %s", buffer);

    close(sd);
}