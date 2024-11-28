
#ifndef RECEIVE_PIECE_H
#define RECEIVE_PIECE_H

#include <netinet/in.h>
#include "meta.h"

// 다운로드 요청 스레드 함수 매개 변수
typedef struct {
    meta meta_data;
    int start_index;
    int end_index;
    char seed_IP[16];
    char temp_file_name[256];
} down_request_targs;

// 피스 파일 요청 구조체
typedef struct {
    char filename[256];
    int64_t start_chunk;
    int64_t end_chunk;
} file_request;

void* receive_piece(void* vargs);
#endif 