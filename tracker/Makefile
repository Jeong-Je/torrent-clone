SRC = tracker.c update_seedlist.c request_thread.c give_seed.c is_seed.c

TARGET = tracker

all:${TARGET}

${TARGET}:${SRC}
	gcc -Wall -o ${TARGET} ${SRC} -lpthread

clean:
	rm ${TARGET}
