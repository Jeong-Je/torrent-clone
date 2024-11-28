#include "env.h"

// key에 해당하는 value를 반환하는 함수
char* get_env(char* key) {
    FILE *file = fopen(".env", "r");

    if (file == NULL) {
        perror("env 파일 열기 실패");
        return NULL; 
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* equal_sign = strchr(line, '=');  // '=' 위치 찾기
        if (equal_sign != NULL) {
            *equal_sign = '\0';  // '='을 null 문자로 변경하여 key와 value 구분

            // line은 key, equal_sign+1은 value
            if (strcmp(line, key) == 0) {
                char* value = strdup(equal_sign + 1); // value는 문자열 복사 후 반환

                // 개행 문자 제거
                size_t len = strlen(value);
                if(len > 0 && value[len - 1] == '\n')
                    value[len - 1] = '\0';
                
                fclose(file);
                return value;
            }
        }
    }

    fclose(file);  // 파일 닫기
    return NULL;  // key를 찾지 못한 경우 NULL 반환
}