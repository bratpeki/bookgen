
# TODO: Currently only working with example.c

CC     := gcc
CFLAGS := -ansi -Wall -Wextra -Wno-unused-function

build:
	@$(CC) $(CFLAGS) example.c -o example.out
	@./example.out
	@-rm example.out

clean:
	@-rm *.html

listfunc:
	@cat bg.h | grep "static void BG_PUBAPI_IMPL"

