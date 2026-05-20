CC = gcc
CFLAGS = -Wall -Wextra -std=c99

build:
	$(CC) $(CFLAGS) -o search_index search_index.c

clean:
	rm -f search_index indexare.out
