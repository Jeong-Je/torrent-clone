#pragma once

typedef struct meta {
        char announce[100];
        char name[100];
        int file_length;
        int piece_length;
        int piece_num;
} meta;
