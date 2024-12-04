PLATFORM=x86_64-lux
CCFLAGS=-Wall -c -O3
LDFLAGS=-llux
CC=x86_64-lux-gcc
LD=x86_64-lux-gcc
SRC:=$(shell find . -type f -name "*.c")
OBJ:=$(SRC:.c=.o)

all: head

%.o: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

head: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m head"
	@$(LD) $(OBJ) -o head $(LDFLAGS)

install: head
	@cp head ../out/

clean:
	@rm -f head $(OBJ)
