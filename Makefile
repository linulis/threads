INC_DIR=/home/vasilil/src/threads

threads: main.c $(INC_DIR)/
	gcc main.c -I$(INC_DIR) -lpthread -o threads.o


mmap: mmap.c $(INC_DIR)/
	gcc mmap.c -I$(INC_DIR) -o mmap.o

all: threads


clean:
	rm -rf *.o

