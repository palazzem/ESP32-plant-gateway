# Makefile to automate some commands

tests:
	pio test -e native

build:
	pio run -e esp32dev

format-check:
	find . -name '*.cpp' -o -name '*.h' | egrep -v ".pio" | xargs clang-format --dry-run -Werror -style=LLVM

format:
	find . -name '*.cpp' -o -name '*.h' | egrep -v ".pio" | xargs clang-format -i -style=LLVM
