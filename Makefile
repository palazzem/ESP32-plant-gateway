# Makefile to automate some commands

tests:
	pio test -e native

build:
	pio run -e esp32dev

format-check:
	clang-format --dry-run -Werror -style=LLVM include/* src/* test/*

format:
	clang-format -i -style=LLVM include/* src/* test/*
