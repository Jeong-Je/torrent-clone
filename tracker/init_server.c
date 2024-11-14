#include "init_server.h"
#include "handle_client.h"

//tracker server configuration file
int init_server(){

        struct sockaddr_in server_address, client_address;
        int server_socket;
	int clientlen = sizeof(client_address);

        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket == -1){

                perror("socket create err"); exit(1);
        }

        server_address.sin_family = AF_INET;
        server_address.sin_port = htons(PORTNUM);
        server_address.sin_addr.s_addr = inet_addr("113.198.138.212");

        if(bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0){

                perror("bind err");
                close(server_socket);
                exit(1);
        }

        if(listen(server_socket, CLIENT_MAX) < 0){

                perror("listen err");
                close(server_socket);
                exit(1);
        }

	while(1){

		int new_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&clientlen);
		
		if(new_socket<0){

			perror("accept err"); exit(1);
		}

		handle_client(&new_socket);
	}

	return 0;
}

