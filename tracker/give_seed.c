#include "give_seed.h"

int give_seed(char* file_name, int new_socket) {
    printf("give_seed 함수 호출\n");

    char* max_client_str = get_env("MAX_CLIENT");
    int max_client = atoi(max_client_str);
    free(max_client_str);

    int seeds_index = 0;
    char line[256];
    char file_path[256];
    char* seeds[max_client];
    FILE* file;

    for (int i = 0; i < max_client; i++) {
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

    if (seed_cnt == 0) {
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
                printf("hi there > %s\n", line);
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

    for (int i = 0; i < max_client; i++) {
        free(seeds[i]); // 메모리 해제
    }

    return 0;
}