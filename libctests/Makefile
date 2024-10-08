PLATFORM=x86_64-lux
CCFLAGS=-Wall -O3
LDFLAGS=-llux
CC=x86_64-lux-gcc
SRC:=$(shell find . -type f -name "*.c")
OUT:=$(SRC:.c=)

all: $(OUT)

%: %.c
	@echo "\x1B[0;1;32m cc  \x1B[0m $<"
	@$(CC) $(CCFLAGS) -o $@ $<

install: $(OUT)
	@cp $(OUT) ../out/

clean:
	@rm -f $(OUT)
