CC = gcc
PP = g++

all: server client

test: serverS

server: server.cpp
	$(PP) -o server server.cpp

serverS: serverS.cpp
	$(PP) -o serverS serverS.cpp

client: client.cpp
	$(PP) -o client client.cpp

clean:
	rm server client
