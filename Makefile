
# TODO: Currently only working with example.c
#       Introduce more examples down the line.

CC     := gcc
CFLAGS := -ansi -Wall -Wextra

build:
	@$(CC) $(CFLAGS) example.c -o example.out
	@./example.out > output.html

clean:
	@-rm *.out *.html

listfunc:
	@cat bg.h | grep "static void BG_"

