all:
	@echo "\x1B[0;1;35m make\x1B[0m utilities/hello"
	@make -C hello

install:
	@mkdir -p out
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/hello"
	@make install -C hello

clean:
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/hello"
	@make clean -C hello
