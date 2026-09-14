
CC = gcc
CC1 = g++ -Wno-write-strings
LOAD_GEN_FILE = load_gen.c
SERVER_FILE = server.cpp
HTTP_SERVER_FILE = http_server.cpp

all: load_gen server

load_gen: $(LOAD_GEN_FILE)
	$(CC) $(LOAD_GEN_FILE) -pthread -o load_gen

server: $(SERVER_FILE) $(HTTP_SERVER_FILE)
	$(CC1) $(SERVER_FILE) $(HTTP_SERVER_FILE) -pthread -ggdb3 -g -o server
	
clean:
	rm -f load_gen server *.log
