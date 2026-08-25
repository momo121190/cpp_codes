CC = gcc
PKGS = gstreamer-1.0 gstreamer-pbutils-1.0
CFLAGS = -Wall -O2 $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --libs $(PKGS)) -lpthread

server: server.c
	$(CC) $(CFLAGS) -o server server.c $(LIBS)
	@echo "OK -> ./server"

clean:
	rm -f server *.mp4

.PHONY: clean
