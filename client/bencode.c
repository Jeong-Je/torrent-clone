#include <stdio.h>
#include <string.h>

#include "bencode.h"

/*
 * BEncode 규칙
 * 문자열 [길이]:[데이터]
 * 숫  자 i[숫자]e
 * 리스트 lstrings or integers]e
 * 사  전 d[keys][values]e
 */

void bencode_dict_start(FILE *fp) {
    fprintf(fp, "d");
}

void bencode_dict_end(FILE *fp) {
    fprintf(fp, "e");
}

void bencode_list_start(FILE *fp) {
    fprintf(fp, "l");
}

void bencode_list_end(FILE *fp) {
    fprintf(fp, "e");
}

void bencode_int(FILE *fp, long long value){
        fprintf(fp, "i%llde", value);
}

void bencode_string(FILE *fp, char* str){
        fprintf(fp, "%zu:%s", strlen(str), str);
}
