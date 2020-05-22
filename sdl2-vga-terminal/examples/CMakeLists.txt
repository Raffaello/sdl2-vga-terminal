cmake_minimum_required (VERSION 3.16)

### Macro ###
macro(macro_example) # ${ARGN} link libraries
    set(prefix EXAMPLE)
    set(flags COPY_LIB NONE)
    set(singleValues EXE FILE)
    set(multiValues DEPS LINKS)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("${EXE} - ${FILE} - ${DEPS} - ${LINKS}")
    #message("EXE====${EXAMPLE_EXE}")
    #message("IS NONE: ${EXAMPLE_NONE}")
    #message("IS CPLIB: ${EXAMPLE_COPY_LIB}")
    #message("EXE: ${EXAMPLE_EXE}")
    #message("FILE: ${EXAMPLE_FILE}")
    #message("DEP: ${EXAMPLE_DEPS}")
    #message("LINKS: ${EXAMPLE_LINKS}")
    add_executable (${EXAMPLE_EXE} ${EXAMPLE_FILE})
    add_dependencies(${EXAMPLE_EXE} ${EXAMPLE_DEPS}) 
    target_link_libraries(${EXAMPLE_EXE} ${EXAMPLE_LINKS})
    if(${EXAMPLE_COPY_LIB})
        #message("===============EXAMPLE COPY LIB")
        foreach(TF IN ITEMS ${EXAMPLE_DEPS})
            #message("++++ TF= ${TF} ++++")
            add_custom_command(
                TARGET ${EXAMPLE_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${TF}> $<TARGET_FILE_DIR:${EXAMPLE_EXE}>
            )
        endforeach()
    endif()
endmacro()

############################## Main Example ##################################
macro_example(
    EXE "${PROJECT_NAME}"
    FILE "sdl2-vga-terminal.cpp"
    DEPS vga-terminal-static
    LINKS ${LIB_SDL2main} vga-terminal-static
    NONE
)

############################### TUI Example ###################################
macro_example(
    EXE "${PROJECT_NAME}-tui"
    FILE "sdl2-tui-terminal.cpp"
    DEPS vga-terminal-static
    LINKS ${LIB_SDL2main} vga-terminal-static
    NONE
)

if (BUILD_SHARED_LIBS)
################## C example using SO/DLL with lazy loading ##################
    macro_example(
        EXE ${PROJECT_NAME}-so
        FILE "sdl2-vga-terminal-so.c"
        DEPS  vga-terminal
        LINKS ${LIB_SDL2main} vga-terminal
        COPY_LIB
	)
    
    if(MSVC)
        target_link_options(${PROJECT_NAME}-so PRIVATE 
            "/INCREMENTAL" 
            "/DELAYLOAD:$<TARGET_FILE_NAME:vga-terminal>" 
            "/DELAY:UNLOAD"
        )
    endif()


################## C example using SO/DLL run-time loading ##################
    macro_example(
     EXE ${PROJECT_NAME}-load-so
     FILE "sdl2-vga-terminal-load-so.c"
     DEPS vga-terminal
     LINKS PUBLIC ${LIB_SDL2main} ${LIB_SDL2} ${CMAKE_DL_LIBS}
	)
    target_link_options(${PROJECT_NAME}-load-so PUBLIC ${LINK_SUBSYS_WIN})
endif()
