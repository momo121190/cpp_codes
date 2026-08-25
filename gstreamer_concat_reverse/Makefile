CC = gcc
CFLAGS = -Wall -O2 $(shell pkg-config --cflags gstreamer-1.0)
LIBS = $(shell pkg-config --libs gstreamer-1.0) -lpthread

all: player

player: player_concat_seek.c
	$(CC) $(CFLAGS) -o player player_concat_seek.c $(LIBS)
	@echo "OK -> ./player"

clean:
	rm -f player *.mp4

.PHONY: all clean
