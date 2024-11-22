
#ifndef RECEIVE_PIECE_H
#define RECEIVE_PIECE_H

#include <netinet/in.h>
#include "meta.h"

void receive_piece(meta meta, int start_index, int end_index, in_addr_t seed_IP, char* temp_file_name);
#endif 