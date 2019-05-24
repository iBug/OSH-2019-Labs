CC := gcc
CFLAGS := -O2 -Wall -Wno-unused-result -Wno-unused-variable -s
LDFLAGS := -lc -lm

DATA_DIR := data
DATA_OBJS :=
OBJS := main.o util.o signals.o globals.o reqhandler.o $(DATA_OBJS)

.PHONY: all clean

all: server

server: $(OBJS)
	${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

%.o: %.c config.h
	${CC} ${CFLAGS} -c -o $@ $<

${DATA_DIR}/%.o: ${DATA_DIR}/%.c
	${CC} -c -o $@ $^

${DATA_DIR}/%.c: ${DATA_DIR}/%
	sh gen_data.sh $*

clean:
	rm -f server *.o
