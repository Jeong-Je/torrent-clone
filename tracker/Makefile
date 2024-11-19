SRC = main.c init_server.c handle_client.c update_seedlist.c is_seed.c

TARGET = main

all:${TARGET}

${TARGET}:${SRC}
	gcc -Wall -o ${TARGET} ${SRC}

clean:
	rm ${TARGET}
