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
	@echo "\x1B[0;1;35m make\x1B[0m utilities/libctests"
	@make -C libctests
	@echo "\x1B[0;1;35m make\x1B[0m utilities/head"
	@make -C head
	@echo "\x1B[0;1;35m make\x1B[0m utilities/reset"
	@make -C reset
	@echo "\x1B[0;1;35m make\x1B[0m utilities/chmod"
	@make -C chmod
	@echo "\x1B[0;1;35m make\x1B[0m utilities/touch"
	@make -C touch
	@echo "\x1B[0;1;35m make\x1B[0m utilities/rm"
	@make -C rm

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
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/libctests"
	@make install -C libctests
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/head"
	@make install -C head
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/reset"
	@make install -C reset
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/chmod"
	@make install -C chmod
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/touch"
	@make install -C touch
	@echo "\x1B[0;1;35m make\x1B[0m install utilities/rm"
	@make install -C rm

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
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/libctests"
	@make clean -C libctests
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/head"
	@make clean -C head
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/reset"
	@make clean -C reset
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/chmod"
	@make clean -C chmod
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/touch"
	@make clean -C touch
	@echo "\x1B[0;1;35m make\x1B[0m clean utilities/rm"
	@make clean -C rm