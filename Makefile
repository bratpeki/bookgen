
# TODO: Currently only working with example.c
#       Introduce more examples down the line.

CC     := gcc
CFLAGS := -ansi -Wall -Wextra

build:
	@$(CC) $(CFLAGS) example.c -o example.out
	@./example.out
	@-rm example.out

clean:
	@-rm *.html

listfunc:
	@cat bg.h | grep "static void BG_"

