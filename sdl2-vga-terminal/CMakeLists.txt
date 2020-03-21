# CMakeList.txt : CMake project for sdl2-vga-terminal, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 14)

find_package(SDL2 CONFIG REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

set(
	SOURCES 
	src/Window.cpp
	src/VgaTerminal.cpp
)

# Add source to this project's executable.
add_executable (sdl2-vga-terminal "sdl2-vga-terminal.cpp" "sdl2-vga-terminal.h" ${SOURCES})

# TODO: Add tests and install targets if needed.

target_link_libraries(sdl2-vga-terminal  SDL2::SDL2 SDL2::SDL2main)
#target_link_libraries(sdl2-vga-terminal  ${SDL2_LIBRARIES} )
