#include <stdio.h>
#include <stdlib.h>

#include "make_torrent.h"

int main(int argc, char *argv[]) {
    if (argc == 2) {
        char *file_path = argv[1];
        create_torrent(file_path, "113.198.138.212");
    } else {
        fprintf(stderr, "사용법: %s <파일 경로>\n", argv[0]);
        return 1;
    }
    return 0;
}

