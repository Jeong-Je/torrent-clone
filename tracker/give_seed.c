#include "give_seed.h"


int give_random(char file_path[256], char* seeds[MAX_SEEDS], int seed_cnt) {
    FILE* file;
    char line[256];
    int seeds_index = 0;

    file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("fopen err");
        exit(1);
    }

    bool* gived_seed = malloc(sizeof(bool) * (seed_cnt + 1)); // 고정
    if (gived_seed == NULL) {
        perror("malloc failed");
        exit(1);
    }

    for (int i = 0; i <= seed_cnt; i++) {
        gived_seed[i] = false;
    }

    srand(time(0));
    for (int i = 0; i < 5; i++) {
        int random_value = rand() % seed_cnt + 1;
        while (gived_seed[random_value]) {
            random_value = rand() % seed_cnt + 1;
        }

        int temp = 1;
        while (temp < random_value && fgets(line, 256, file)) {
            temp++;
        }

        fgets(line, 256, file); // 랜덤한 줄 읽기

        seeds[seeds_index] = malloc(256 * sizeof(char)); // 메모리 할당
        if (seeds[seeds_index] == NULL) {
            perror("malloc failed");
            exit(1);
        }

		int len = strlen(line); line[len-1] = '\0';
        strcpy(seeds[seeds_index], line);

        gived_seed[random_value] = true;
        memset(line, '\0', sizeof(line));
        fseek(file, 0, SEEK_SET);
        seeds_index++;
    }

    free(gived_seed);
    fclose(file);
    return seeds_index;
}

int give_seed(char* file_name, int new_socket) {
    printf("give_seed 함수 호출\n");

    int seeds_index = 0;
    char line[256];
    char file_path[256];
    char* seeds[MAX_SEEDS];
    FILE* file;

    for (int i = 0; i < MAX_SEEDS; i++) {
        seeds[i] = NULL;
    }

    snprintf(file_path, sizeof(file_path), "./seed_List/%s", file_name);

    file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("There is not seeds.\n");
        return -1;
    }

    int seed_cnt = 0;
    while (fgets(line, sizeof(line), file)) {
        seed_cnt++;
    }

    printf("seed_cnt : %d\n", seed_cnt);

    if (seed_cnt > 5) {
        seeds_index = give_random(file_path, seeds, seed_cnt);
    } else if (seed_cnt == 0) {
        printf("There is not seeds.\n");
    } else {
        fseek(file, 0, SEEK_SET);
        for (int i = 0; i < seed_cnt; i++) {
            if (fgets(line, sizeof(line), file)) {
                seeds[i] = malloc(256 * sizeof(char));
                if (seeds[i] == NULL) {
                    perror("malloc failed");
                    exit(1);
                }
				
				int len = strlen(line); line[len-1] = '\0';
                strcpy(seeds[i], line);
            }
        }
        seeds_index = seed_cnt;
    }

    char result[1024] = "";
    sprintf(result, "%d@", seeds_index); // 제일 처음 토큰은 피어의 수
    for (int i = 0; i < seeds_index; i++) {
        printf("seeds %d : %s\n", i, seeds[i]);
        if (i == 0) {
            snprintf(result, sizeof(result), "%d@%s", seeds_index,seeds[i]);
        } else {
            char temp_result[1024];
            snprintf(temp_result, sizeof(temp_result), "%s,%s", result, seeds[i]);
            strncpy(result, temp_result, sizeof(result));
        }
    }

    printf("result : %s\n", result);	//test

	if(send(new_socket, result, strlen(result)+1, 0) == -1){

		perror("send err"); exit(1);
	}

    fclose(file);

    for (int i = 0; i < MAX_SEEDS; i++) {
        free(seeds[i]); // 메모리 해제
    }

    return 0;
}