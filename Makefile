# Makefile to automate some commands

tests:
	pio test -e native

build:
	pio run -e esp32dev

format-check:
	clang-format --dry-run -Werror -i -style=LLVM include/config.h

format:
	clang-format -i -style=LLVM include/config.h
