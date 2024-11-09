TARGET = client
SRC = main.c create_torrent.c bencode.c

all: ${TARGET}

${TARGET}: ${SRC}
	gcc -Wall -o ${TARGET} ${SRC}

clean:
	rm ${TARGET}
