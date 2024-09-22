PLATFORM=x86_64-lux
CCFLAGS=-Wall -c -O3
LDFLAGS=-llux
CC=x86_64-lux-gcc
LD=x86_64-lux-gcc
SRC:=$(shell find . -type f -name "*.c")
OBJ:=$(SRC:.c=.o)

all: hello

%.o: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

hello: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m hello"
	@$(LD) $(OBJ) -o hello $(LDFLAGS)

install: hello
	@cp hello ../out/

clean:
	@rm -f hello $(OBJ)
