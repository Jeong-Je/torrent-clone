
#ifndef RECEIVE_PIECE_H
#define RECEIVE_PIECE_H

#include <netinet/in.h>
#include "meta.h"

// 다운로드 요청 스레드 매개 변수
typedef struct {
    meta meta_data;
    int start_index;
    int end_index;
    char seed_IP[16];
    char temp_file_name[256];
} thread_args;

void* receive_piece(void* vargs);
#endif 