#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


char* request_tracker(char *announce, char *file_name, int* peer_num){
    char *ip = strtok(announce, ":"); // ":" 문자를 기준으로 앞 문자열 (ip 주소)
    char *port = strtok(NULL, ":"); // ":" 문자를 기준으로 두 번째 문자열 (port 번호)

    int sd;
    char buffer[256];
    struct sockaddr_in sin;

    sprintf(buffer, "give_seed:%s.torrent", file_name);
    printf("%s\n", buffer); ///test 

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

    printf("buffer: %s\n", buffer);

    char* peer_num_buf = strtok(buffer, "@");
    *peer_num = atoi(peer_num_buf);

    char** ip_arr = (char**)malloc(sizeof(char*) * (*peer_num));
    for (int i=0; i<*peer_num; i++){
        ip_arr[i] = NULL;
    }

    char* ptr = strtok(NULL, ",");
    int i = 0;
    while (ptr != NULL){
        ip_arr[i] = ptr;
        ptr = strtok(NULL, ",");
        i++;
    }

    printf("peernum: %d\n", *peer_num);
    for (int i=0; i<5; i++){
        if(ip_arr[i] == NULL) break;
           
        printf("ptr: %s\n", ip_arr[i]);
    }

    close(sd);
    printf("End\n");
    return ip_arr;
}