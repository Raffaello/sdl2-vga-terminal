cmake_minimum_required (VERSION 3.16)

############################## Main Example ##################################
add_executable (${PROJECT_NAME} "sdl2-vga-terminal.cpp")
add_dependencies(${PROJECT_NAME} vga-terminal-static) 
target_link_libraries(${PROJECT_NAME} vga-terminal-static ${LIB_SDL2main})


################## C example using SO/DLL with lazy loading ##################
add_executable (${PROJECT_NAME}-so "sdl2-vga-terminal-so.c")
add_dependencies(${PROJECT_NAME}-so vga-terminal) 
target_link_libraries(${PROJECT_NAME}-so vga-terminal ${LIB_SDL2main})
add_custom_command(TARGET "${PROJECT_NAME}-so" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:vga-terminal> $<TARGET_FILE_DIR:${PROJECT_NAME}-so>
)
if(MSVC)
    target_link_options(${PROJECT_NAME}-so PRIVATE 
        "/INCREMENTAL" 
        "/DELAYLOAD:$<TARGET_FILE_NAME:vga-terminal>" 
        "/DELAY:UNLOAD"
    )
endif()


################## C example using SO/DLL run-time loading ##################
add_executable (${PROJECT_NAME}-load-so "sdl2-vga-terminal-load-so.c")
target_link_libraries(${PROJECT_NAME}-load-so PUBLIC ${LIB_SDL2} ${LIB_SDL2main} ${CMAKE_DL_LIBS})
add_custom_command(TARGET "${PROJECT_NAME}-load-so" POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:vga-terminal> $<TARGET_FILE_DIR:${PROJECT_NAME}-load-so>
)
