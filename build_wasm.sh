#/bin/bash

### More as a reminder to build it.

mkdir build
cd build
## emcc need to be in the path. (run source emcsdk_env.sh)
emcc -std=c++17 ../sdl2-vga-terminal/examples/wasm.cpp ../sdl2-vga-terminal/src/Window.cpp ../sdl2-vga-terminal/src/VgaTerminal.cpp -I ../sdl2-vga-terminal/src/ -s USE_SDL=2  -o v.html
python -m http.server 8080

