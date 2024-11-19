#ifndef GIVE_SEED_H
#define GIVE_SEED_H

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<time.h>

#define MAX_SIZE 1024

int give_seed(const char* file_id);

int give_seed_random(int seed_count, char line[], FILE* seedlist);

#endif
