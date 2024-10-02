PLATFORM=x86_64-lux
CCFLAGS=-Wall -c -I./src/include -O3
LDFLAGS=
CC=x86_64-lux-gcc
LD=x86_64-lux-gcc
SRC:=$(shell find ./src -type f -name "*.c")
OBJ:=$(SRC:.c=.o)

all: lush

%.o: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

lush: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m lush"
	@$(LD) $(OBJ) -o lush $(LDFLAGS)

install: lush
	@cp lush ../out/

clean:
	@rm -f lush $(OBJ)
