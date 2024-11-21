#define FILE_SIZE 256
#define PIECE_LENGTH 131072 // 128KB

#include "bencode.h"
#include "env.h"

void create_torrent(char *file_path, char *torrent_file);
