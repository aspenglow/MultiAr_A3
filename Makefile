
CC = gcc
CFLAGS = -std=gnu99 -O3 -fopenmp -Wall

all: numa order

numa: numa.c utility.h
	$(CC) $(CFLAGS) $< -o $@

order: order.c
	$(CC) $(CFLAGS) -lpthread $< -o $@

order_asm: order.c
	$(CC) $(CFLAGS) -lpthread -S $< -o $@

clean:
	rm -f numa order
