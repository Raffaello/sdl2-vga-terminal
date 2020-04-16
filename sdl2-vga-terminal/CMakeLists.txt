cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

############################ Shared lib ######################################
include(GenerateExportHeader)
add_library(vga-terminal-lib SHARED)
generate_export_header(
    vga-terminal-lib
    BASE_NAME vga-terminal-lib
    EXPORT_MACRO_NAME VGA_TERMINAL_EXPORT
    EXPORT_FILE_NAME vga-terminal-export.h
    STATIC_DEFINE vga-terminal-lib_BUILD_AS_STATIC
)
install(
    TARGETS vga-terminal-lib 
    EXPORT vga-terminal-lib-config
    DESTINATION lib
    PUBLIC_HEADER DESTINATION inc
)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vga-terminal.h
    $<TARGET_FILE_DIR:vga-terminal-lib>/vga-terminal-export.h 
    DESTINATION inc
)
export(TARGETS vga-terminal-lib
    FILE ${CMAKE_BINARY_DIR}/vga-terminal.cmake
)
install(EXPORT
    vga-terminal-lib-config DESTINATION cmake
)
target_include_directories(vga-terminal-lib PUBLIC 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:vga-terminal-lib>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_sources(vga-terminal-lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vga-terminal.cpp
)
target_link_libraries(vga-terminal-lib SDL2::SDL2 SDL2::SDL2main)


################ Shared code among exe and tests #############################
add_library(vga-terminal OBJECT)
target_sources(vga-terminal PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
)

target_include_directories(vga-terminal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(vga-terminal PUBLIC SDL2::SDL2 SDL2::SDL2main)


############################## Main Example ##################################
add_executable (${PROJECT_NAME} "sdl2-vga-terminal.cpp")
add_dependencies(${PROJECT_NAME} vga-terminal) 
target_link_libraries(${PROJECT_NAME} vga-terminal)


################## C example using SO/DLL with lazy loading ##################
add_executable (${PROJECT_NAME}-so "sdl2-vga-terminal-so.c")
add_dependencies(${PROJECT_NAME}-so vga-terminal-lib) 
target_link_libraries(${PROJECT_NAME}-so vga-terminal-lib)
if(MSVC)
    target_link_options(${PROJECT_NAME}-so PRIVATE 
        "/INCREMENTAL" 
        "/DELAYLOAD:$<TARGET_FILE_NAME:vga-terminal-lib>" 
        "/DELAY:UNLOAD"
    )
endif()


################## C example using SO/DLL run-time loading ##################
add_executable (${PROJECT_NAME}-load-so "sdl2-vga-terminal-load-so.c")
target_link_libraries(${PROJECT_NAME}-load-so PUBLIC SDL2::SDL2 SDL2::SDL2main)


############################### tests
add_subdirectory("test")
