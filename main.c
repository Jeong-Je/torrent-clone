#include<stdio.h>
#include<stdlib.h>

#include "handle_client.h"
#include "init_server.h"

int main(int argc, char* argv[]){

	if(argc < 2){
	
		printf("please enter command");
		return 0;
	}

	char* command = argv[1];
	init_server(command);

	return 0;
}
