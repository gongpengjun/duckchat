CC = gcc
PP = g++

all: server client

server: server.cpp
	$(PP) -o server server.cpp

client: client.c
	$(CC) -o client client.c

clean:
	rm server client
