cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)
## Shared code among exe and tests
add_library(vga-terminal OBJECT)
target_sources(vga-terminal PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
)

target_include_directories(vga-terminal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

option(USE_PACKAGE_LIBS "using precompiled libs from os (only Linux/Unix)" OFF)

if(USE_PACKAGE_LIBS AND UNIX)
    target_link_libraries(vga-terminal PUBLIC  ${SDL2_LIBRARIES})
else()
    target_link_libraries(vga-terminal PUBLIC SDL2::SDL2 SDL2::SDL2main)
endif()

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "sdl2-vga-terminal.cpp")
target_link_libraries(${PROJECT_NAME} vga-terminal)
target_include_directories(${PROJECT_NAME} PRIVATE .)
#target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)

# tests
add_subdirectory("test")
