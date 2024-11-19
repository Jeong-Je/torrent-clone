#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "meta.h"
#include "allocate_storage.h"

void allocate_storage(meta meta, char* temp_file_name){
    sprintf(temp_file_name, "%s_temp_download_file", meta.name);
    int fd = open(temp_file_name, O_CREAT | O_WRONLY, 0644);
    if (fd == -1){
        perror("파일을 생성하지 못하였습니다.\n");
        exit(1);
    }
    //fallocate(fd, 0, 0, meta.file_length);

    close(fd);
}