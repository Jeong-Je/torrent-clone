#include "update_seedlist.h"

int update_seedlist(const char* file_id, const char* client_ip){

	char filename[1024];

	snprintf(filename, sizeof(filename), "%s_seedlist.txt", file_id);

	FILE* seedlist = fopen(filename, "a");
	if(seedlist == NULL){
	
		perror("fopen err"); exit(1);
	}

	fprintf(seedlist, "%s", client_ip);

	fclose(seedlist);

	return 0;
}
