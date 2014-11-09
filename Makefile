CC = gcc
PP = g++

all: server client

test: serverT

server: server.cpp
	$(PP) -o server server.cpp

serverT: serverT.cpp
	$(PP) -o serverT serverT.cpp

client: client.cpp
	$(PP) -o client client.cpp

clean:
	rm server client
