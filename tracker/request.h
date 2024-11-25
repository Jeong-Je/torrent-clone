#ifndef REQUEST_H
#define REQUEST_H

typedef struct { // 클라 요청에서 filename과 client_ip를 담을 구조체
	char file_name[100];
	char client_ip[16];
} Request;

#endif
