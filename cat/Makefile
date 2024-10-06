PLATFORM=x86_64-lux
CCFLAGS=-Wall -c -O3
LDFLAGS=-llux
CC=x86_64-lux-gcc
LD=x86_64-lux-gcc
SRC:=$(shell find . -type f -name "*.c")
OBJ:=$(SRC:.c=.o)

all: cat

%.o: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

cat: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m cat"
	@$(LD) $(OBJ) -o ls $(LDFLAGS)

install: cat
	@cp cat ../out/

clean:
	@rm -f cat $(OBJ)
