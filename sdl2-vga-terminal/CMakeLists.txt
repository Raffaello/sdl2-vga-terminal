cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

## Shared code among exe and tests
add_library(vga-terminal OBJECT)
target_sources(vga-terminal PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
)

target_include_directories(vga-terminal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

if (MSVC)
    target_link_libraries(vga-terminal PUBLIC SDL2::SDL2 SDL2::SDL2main)
elseif (UNIX)
    target_link_libraries(vga-terminal PUBLIC  ${SDL2_LIBRARIES})
endif()

# SO lib
include(GenerateExportHeader)
add_library(vga-terminal-lib SHARED)
add_dependencies(vga-terminal-lib vga-terminal)
generate_export_header(
    vga-terminal-lib
    BASE_NAME vga-terminal-lib
    EXPORT_MACRO_NAME VGA_TERMINAL_EXPORT
    EXPORT_FILE_NAME vga-terminal-export.h
    STATIC_DEFINE vga-terminal-lib_BUILD_AS_STATIC
)
install(
    TARGETS vga-terminal-lib DESTINATION lib
    PUBLIC_HEADER DESTINATION inc
)
install(FILES 
   "${CMAKE_CURRENT_SOURCE_DIR}/include/vga-terminal.h"
    $<TARGET_FILE_DIR:vga-terminal-lib>/vga-terminal-export.h 
    DESTINATION inc #${INCLUDE_INSTALL_DIR}
)
target_include_directories(vga-terminal-lib PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:vga-terminal-lib>>
)
target_sources(vga-terminal-lib PRIVATE
    $<TARGET_OBJECTS:vga-terminal> 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vga-terminal.c
)
target_link_libraries(vga-terminal-lib vga-terminal)


# Add source to this project's executable.
add_executable (${PROJECT_NAME} "sdl2-vga-terminal.cpp")
add_dependencies(${PROJECT_NAME} vga-terminal) 
target_link_libraries(${PROJECT_NAME} vga-terminal)


# using the DLL with lazy loading, so it can be unloaded.
add_executable (${PROJECT_NAME}-so "sdl2-vga-terminal-so.cpp")
#add_dependencies(${PROJECT_NAME}-so vga-terminal-lib) 
target_link_libraries(${PROJECT_NAME}-so vga-terminal-lib)
target_include_directories(${PROJECT_NAME}-so
   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
# The location of the headers before installation 
    #$<BUILD_INTERFACE: $<TARGET_FILE_DIR:vga-terminal-lib>>
)

if(MSVC)

#add_compile_options("/link /DELAYLOAD:vga-terminal-libd.dll")
#add_definitions(-DCMAKE_EXE_LINKER_FLAGS=" /DELAYLOAD:vga-terminal-libd.dll /DELAY:UNLOAD")
#set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY LINK_FLAGS " /DELAYLOAD:vga-terminal-libd.dll /DELAY:UNLOAD")
target_link_options(${PROJECT_NAME}-so PUBLIC "/DELAYLOAD:$<TARGET_FILE:${PROJECT_NAME}-so>" "/DELAY:UNLOAD")
endif()
# tests
add_subdirectory("test")
