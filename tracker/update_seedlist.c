#include "update_seedlist.h"

int update_seedlist(const char* file_id, const char* client_ip){

	FILE* seedlist = fopen("seedlist.txt", "a");
	
	fprintf(seedlist, "%s:%s\n", file_id, client_ip);

	fclose(seedlist);

	return 0;
}
