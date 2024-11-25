#ifndef IS_SEED_H
#define IS_SEED_H

#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "request.h"

#define MAX_LEN_LENGTH 1024

bool is_seed(Request* req, const char* file_path);

#endif
