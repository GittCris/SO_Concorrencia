CC=gcc
CFLAGS=-pthread

all: run

sushi: signal.c
	$(CC) $(CFLAGS) -o sushi signal.c

run: sushi
	./sushi

clean:
	rm -f sushi
