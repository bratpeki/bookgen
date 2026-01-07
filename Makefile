
# TODO: Currently only working with example.c
#       Introduce more examples down the line.

CC     := gcc
CFLAGS := -ansi -Wall -Wextra

all:
	$(CC) $(CFLAGS) example.c -o example.out
	./example.out > output.html

