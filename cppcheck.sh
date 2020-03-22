#!/bin/bash

cppcheck --enable=all  --force  -I sdl2-vga-terminal/include/ -I sdl2-vga-terminal/src/  . > /dev/null
