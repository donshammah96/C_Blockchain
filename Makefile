CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lssl -lcrypto
OBJS = blockchain.o network.o main.o

all: blockchain

blockchain: $(OBJS)
	$(CC) $(CFLAGS) -o blockchain $(OBJS) $(LDFLAGS)

blockchain.o: blockchain.c blockchain.h
	$(CC) $(CFLAGS) -c blockchain.c

network.o: network.c blockchain.h
	$(CC) $(CFLAGS) -c network.c

main.o: main.c blockchain.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f blockchain $(OBJS)