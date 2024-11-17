TARGET = torrent
SRC = main.c make_torrent.c bencode.c bdecode.c download_piece.c allocate_storage.c

all: ${TARGET}

${TARGET}: ${SRC}
	gcc -Wall -o ${TARGET} ${SRC}

clean:
	rm ${TARGET}
