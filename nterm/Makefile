PLATFORM=x86_64-lux
CCFLAGS=-Wall -c -I./src/include -O3
LDFLAGS=
CC=x86_64-lux-gcc
LD=x86_64-lux-gcc
SRC:=$(shell find ./src -type f -name "*.c")
OBJ:=$(SRC:.c=.o)

all: nterm

%.o: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

nterm: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m nterm"
	@$(LD) $(OBJ) -o nterm $(LDFLAGS)

install: nterm
	@cp nterm ../out/

clean:
	@rm -f nterm $(OBJ)
