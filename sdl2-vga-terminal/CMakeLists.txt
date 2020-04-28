cmake_minimum_required (VERSION 3.16)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

configure_file(version.h.in version.h)


########################### Static Lib #######################################
add_library(vga-terminal-static STATIC)
install(
    TARGETS vga-terminal-static
    EXPORT vga-terminal-static-config
    DESTINATION static/lib
    PUBLIC_HEADER DESTINATION static/include
)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.hpp
    DESTINATION static/include
)

export(TARGETS vga-terminal-static
    FILE ${CMAKE_BINARY_DIR}/vga-terminal-static.cmake
)
install(EXPORT
    vga-terminal-static-config DESTINATION static/cmake
)
target_include_directories(vga-terminal-static PUBLIC 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:vga-terminal-static>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:static/include>
)
target_sources(vga-terminal-static PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
)

if(WITH_SDL2_STATIC)
    target_link_libraries(vga-terminal-static PUBLIC SDL2::SDL2-static)
else()
    install(FILES
        $<TARGET_FILE:SDL2::SDL2>
        DESTINATION static/lib
    )
    target_link_libraries(vga-terminal-static PUBLIC SDL2::SDL2)
endif()


############################ Shared lib ######################################
include(GenerateExportHeader)
add_library(vga-terminal SHARED)
add_dependencies(vga-terminal vga-terminal-static)
generate_export_header(
    vga-terminal
    BASE_NAME vga-terminal
    EXPORT_MACRO_NAME VGA_TERMINAL_EXPORT
    EXPORT_FILE_NAME vga-terminal-export.h
    STATIC_DEFINE vga-terminal-lib_BUILD_AS_STATIC
)
install(
    TARGETS vga-terminal 
    EXPORT vga-terminal-config
    DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vga-terminal.h
    $<TARGET_FILE_DIR:vga-terminal>/vga-terminal-export.h 
    DESTINATION include
)
install(FILES
    $<TARGET_FILE:SDL2::SDL2>
    DESTINATION lib
)
export(TARGETS vga-terminal 
    FILE ${CMAKE_BINARY_DIR}/vga-terminal.cmake
)
install(EXPORT
    vga-terminal-config DESTINATION cmake
)
target_include_directories(vga-terminal PUBLIC 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:vga-terminal>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_sources(vga-terminal PRIVATE
    #$<BUILD_INTERFACE:vga-terminal-static>
    #${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vga-terminal.cpp
)
target_link_libraries(vga-terminal vga-terminal-static)


######################## add compiler options ################################
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
# @see https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=vs-2019

    target_compile_options(vga-terminal PRIVATE /W4 /WX)
    target_compile_options(vga-terminal-static PRIVATE /W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(vga-terminal PRIVATE -Wall -Wextra -pedantic) #-Werror)
    target_compile_options(vga-terminal-static PRIVATE -Wall -Wextra -pedantic) #-Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
endif()


get_target_property(vga-terminal_copts vga-terminal COMPILE_OPTIONS)
get_target_property(vga-terminal-static_copts vga-terminal-static COMPILE_OPTIONS)

message("target compiler options:")
message(STATUS "vga-terminal " ${vga-terminal_copts})
message(STATUS "vga-terminal-static " ${vga-terminal-static_copts})


if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

################################ Examples ####################################
if(WITH_EXAMPLES)
    add_subdirectory("examples")
endif()

############################### tests ########################################
if(WITH_TEST)
add_subdirectory("test")
endif()
