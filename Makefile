INC_DIR=

threads: main.c $(INC_DIR)/
	gcc main.c -I$(INC_DIR) -lpthread -o threads.o


mmap: mmap.c $(INC_DIR)/
	gcc mmap.c bibl.c -I$(INC_DIR) -o mmap.out

all: threads


clean:
	rm -rf *.o

