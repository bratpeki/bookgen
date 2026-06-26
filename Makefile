
CC     := gcc
CFLAGS := -ansi -Wall -Wextra -Wno-unused-function

build:
	@mkdir -p out
	@$(CC) $(CFLAGS) example.c -o example.out
	@./example.out
	@-rm example.out

clean:
	@-rm -rf out/*
	@-rmdir out

listfunc:
	@cat bg.h | grep "static void BG_PUBAPI_IMPL"

