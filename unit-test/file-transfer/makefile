CC=gcc
CXX=g++

#------------------- MAKE -------------------#

all: client server

client:
	$(CC) $(TP)client.c -o client

server:
	$(CC) $(TP)server.c -o server

clean:
	rm client
	rm server
	rm file