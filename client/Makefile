TARGET = torrent
SRC = client.c make_torrent.c bencode.c bdecode.c allocate_storage.c report_to_tracker.c env.c send_pieces.c receive_piece.c request_tracker.c

all: ${TARGET}

${TARGET}: ${SRC}
	gcc -Wall -o ${TARGET} ${SRC} -lpthread

clean:
	rm ${TARGET}
