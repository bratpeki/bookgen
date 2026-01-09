
# TODO: Currently only working with example.c
#       Introduce more examples down the line.

CC     := gcc
CFLAGS := -ansi -Wall -Wextra

compile:
	@$(CC) $(CFLAGS) example.c -o example.out
	@./example.out > output.html

listfunc:
	@cat bg.h | grep "static void BG_"

