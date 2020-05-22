#!/bin/bash

cpplint --recursive --linelength=120 \
	--exclude=sdl2-vga-terminal/src/vgapalette.h \
	--filter=-legal/copyright,-build/include_order,-whitespace/braces\
,-whitespace/line_length,-build/include_subdir,-whitespace/indent\
,-whitespace/newline,-build/namespaces,-readability/braces,-build/c++11\
,-readability/todo,-whitespace/comments,-readability/utf8\
,-runtime/threadsafe_fn,-runtime/indentation_namespace \
	sdl2-vga-terminal/
