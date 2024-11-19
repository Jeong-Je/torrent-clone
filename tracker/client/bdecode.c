#include <stdio.h>
#include <string.h>
#include "bdecode.h"

bool bdecode(meta *meta, char* file_path) {
	FILE *fp = fopen(file_path, "r");
	if(fp==NULL){
		// 파일 오픈 실패
		fprintf(stderr, "%s open failed\n", file_path);
		fclose(fp);
		return false;
	}

	int size;

    fscanf(fp, "d8:announce%d", &size);
	fgetc(fp); // ":" 한칸 뛰기
	fgets(meta->announce, size+1, fp);

	fscanf(fp, "d4:name%d", &size);
	fgetc(fp);
	fgets(meta->name, size+1, fp);

	fscanf(fp, "11:file_lengthi%de", &size);
	meta->file_length = size;

	fscanf(fp, "12:piece_lengthi%de", &size);
	meta->piece_length = size;

	fscanf(fp, "9:piece_numi%de", &size);
	meta->piece_num = size;

	return true;
}
