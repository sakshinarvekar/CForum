# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Targets
all: cforum server client

# Build the standalone single-user forum
cforum: main.o forum.o ui.o
	$(CC) $(CFLAGS) -o cforum main.o forum.o ui.o

main.o: main.c forum.h
forum.o: forum.c forum.h
ui.o: ui.c forum.h

# Build the server (multi-user)
server: server.o forum.o
	$(CC) $(CFLAGS) -o server server.o forum.o -lpthread

server.o: server.c forum.h

# Build the client
client: client.o
	$(CC) $(CFLAGS) -o client client.o

client.o: client.c

# Clean everything
clean:
	rm -f *.o cforum client server


# all: cforum

# cforum: main.o forum.o ui.o
# 	$(CC) $(CFLAGS) -o cforum main.o forum.o ui.o

# main.o: main.c forum.h
# forum.o: forum.c forum.h
# ui.o: ui.c forum.h

# clean:
# 	rm -f *.o cforum
