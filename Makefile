all:
	@echo "\x1B[0;1;35m make\x1B[0m utilities/hello"
	@make -C hello
	@echo "\x1B[0;1;35m make\x1B[0m utilities/nterm"
	@make -C nterm
	@echo "\x1B[0;1;35m make\x1B[0m utilities/lush"
	@make -C lush
	@echo "\x1B[0;1;35m make\x1B[0m utilities/echo"
	@make -C echo
	@echo "\x1B[0;1;35m make\x1B[0m utilities/pwd"
	@make -C pwd
	@echo "\x1B[0;1;35m make\x1B[0m utilities/ls"
	@make -C ls
	@echo "\x1B[0;1;35m make\x1B[0m utilities/cat"
	@make -C cat
	@echo "\x1B[0;1;35m make\x1B[0m utilities/luxfetch"
	@make -C luxfetch

install:
	@mkdir -p out
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/hello"
	@make install -C hello
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/nterm"
	@make install -C nterm
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/lush"
	@make install -C lush
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/echo"
	@make install -C echo
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/pwd"
	@make install -C pwd
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/ls"
	@make install -C ls
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/cat"
	@make install -C cat
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/luxfetch"
	@make install -C luxfetch

clean:
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/hello"
	@make clean -C hello
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/nterm"
	@make clean -C nterm
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/lush"
	@make clean -C lush
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/echo"
	@make clean -C echo
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/pwd"
	@make clean -C pwd
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/ls"
	@make clean -C ls
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/cat"
	@make clean -C cat
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/luxfetch"
	@make clean -C luxfetch