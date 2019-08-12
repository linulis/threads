INC_DIR=/home/vasilil/src/threads

threads: main.c $(INC_DIR)/
	gcc main.c -I$(INC_DIR) -lpthread -o threads.o

all: threads


clean:
	rm -rf *.o

