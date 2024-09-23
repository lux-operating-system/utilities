all:
	@echo "\x1B[0;1;35m make\x1B[0m utilities/hello"
	@make -C hello
	@echo "\x1B[0;1;35m make\x1B[0m utilities/nterm"
	@make -C nterm

install:
	@mkdir -p out
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/hello"
	@make install -C hello
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/nterm"
	@make install -C nterm

clean:
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/hello"
	@make clean -C hello
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/nterm"
	@make clean -C nterm