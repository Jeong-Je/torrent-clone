TARGET = torrent
SRC = main.c make_torrent.c bencode.c bdecode.c

all: ${TARGET}

${TARGET}: ${SRC}
	gcc -Wall -o ${TARGET} ${SRC}

clean:
	rm ${TARGET}
