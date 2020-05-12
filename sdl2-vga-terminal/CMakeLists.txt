cmake_minimum_required (VERSION 3.16)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

configure_file(version.h.in version.h)

if(WITH_SDL2_STATIC)
    set(LIB_SDL2 SDL2::SDL2-static)
else()
    set(LIB_SDL2 SDL2::SDL2)
endif()

if (WIN32)
    set(LIB_SDL2main SDL2::SDL2main)
endif()

########################### Static Lib #######################################
set(LIB_NAME vga-terminal-static)
set(LIB_CONF ${LIB_NAME}-config)
set(LIB_VER ${LIB_CONF}-version)
include(CMakePackageConfigHelpers)

add_library(${LIB_NAME} STATIC)
install(
    TARGETS ${LIB_NAME}
    EXPORT ${LIB_CONF}
    DESTINATION static/lib
    PUBLIC_HEADER DESTINATION static/include
)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.hpp
    DESTINATION static/include
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
  )

export(TARGETS ${LIB_NAME}
    FILE ${CMAKE_BINARY_DIR}/${LIB_NAME}.cmake
    NAMESPACE VgaTerminal::
)

install(EXPORT ${LIB_CONF}
    DESTINATION static/cmake
    NAMESPACE VgaTerminal::
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
    DESTINATION static/cmake
)
target_include_directories(${LIB_NAME} PUBLIC 
    $<BUILD_INTERFACE:$<TARGET_FILE_DIR:${LIB_NAME}>>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:static/include>
)
target_sources(${LIB_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.cpp
)

target_link_libraries(${LIB_NAME} PUBLIC ${LIB_SDL2})
if(NOT (WITH_SDL2_STATIC))
    install(FILES
        $<TARGET_FILE:SDL2::SDL2>
        DESTINATION static/lib
    )
endif()

unset(LIB_NAME)
unset(LIB_CONF)
unset(LIB_VER)

############################ Shared lib ######################################
if (BUILD_SHARED_LIBS)
    set(LIB_NAME vga-terminal)
    set(LIB_CONF ${LIB_NAME}-config)
    set(LIB_VER ${LIB_CONF}-version)

    include(GenerateExportHeader)

    add_library(${LIB_NAME} SHARED)
    add_dependencies(${LIB_NAME} vga-terminal-static)
    generate_export_header(
        ${LIB_NAME}
        BASE_NAME ${LIB_NAME}
        EXPORT_MACRO_NAME VGA_TERMINAL_EXPORT
        EXPORT_FILE_NAME ${LIB_NAME}-export.h
        STATIC_DEFINE ${LIB_NAME}-lib_BUILD_AS_STATIC
    )
    install(
        TARGETS ${LIB_NAME} 
        EXPORT  ${LIB_CONF}
        DESTINATION lib
        PUBLIC_HEADER DESTINATION include
    )
    install(FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vga-terminal.h
        $<TARGET_FILE_DIR:vga-terminal>/${LIB_NAME}-export.h 
        DESTINATION include
    )
    if (NOT (WITH_SDL2_STATIC))
        install(FILES
            $<TARGET_FILE:SDL2::SDL2>
            DESTINATION lib
        )
    endif()

    write_basic_package_version_file(
      ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
      VERSION ${CMAKE_PROJECT_VERSION}
      COMPATIBILITY AnyNewerVersion
    )

    export(TARGETS vga-terminal 
        FILE ${CMAKE_BINARY_DIR}/${LIB_NAME}.cmake
    )
    install(EXPORT
        ${LIB_CONF} DESTINATION cmake
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIB_VER}.cmake
        DESTINATION cmake
    )
    target_include_directories(${LIB_NAME} PUBLIC 
        $<BUILD_INTERFACE:$<TARGET_FILE_DIR:${LIB_NAME}>>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_sources(${LIB_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vga-terminal.cpp
    )
    target_link_libraries(${LIB_NAME} vga-terminal-static)

    unset(LIB_NAME)
    unset(LIB_CONF)
    unset(LIB_VER)
endif()

### TODO include GCC/CLang sanitizers [refactor the blocks belows]
### BODY: make valgrind test to be compiled in UNIX systems
###       so they can be tested with the sanitizer too and not just with valgrind.

######################## add compiler options ################################
if(CMAKE_CXX_COMPILER_ID MATCHES "^MSVC")
# @see https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=vs-2019

    target_compile_options(vga-terminal-static PUBLIC /W4 /WX
        $<$<CONFIG:DEBUG>:/Od /GR /GS /Gs /guard:cf /RTC1c /D_ALLOW_RTCc_IN_STL>
        $<$<CONFIG:RELEASE>:/O2 /Ot /GA /GL /Gw>
    )
    #if (BUILD_SHARED_LIBS)
    #    target_compile_options(vga-terminal PRIVATE /W4 /WX
    #        $<$<CONFIG:DEBUG>:/Od /GR /GS /Gs /guard:cf /RTC1c /D_ALLOW_RTCc_IN_STL>
    #        $<$<CONFIG:RELEASE>:/O2 /Ot /GA /GL /Gw>
    #    )
    #endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^GNU")
    target_compile_options(vga-terminal-static PUBLIC -Wall -Wextra -pedantic -fPIC 
        $<$<CONFIG:DEBUG>:
            -O0;-D_GLIBCXX_DEBUG;-D_GLIBCXX_ASSERTIONS;-fexceptions;
            -W; -Wshadow; -Wunused-variable;
            -Wunused-parameter; -Wunused-function; -Wunused; -Wno-system-headers;
            -Wno-deprecated; -Woverloaded-virtual; -Wwrite-strings; -fprofile-arcs; -ftest-coverage;
        >
        #$<$<CONFIG:RELEASE>:-O2;-fexceptions;-fstack-clash-protection;-D_FORTIFY_SOURCE=2>
        $<$<CONFIG:RELEASE>:-O2;-fexceptions;-D_FORTIFY_SOURCE=2>
    ) #-Werror)
    target_link_options(vga-terminal-static INTERFACE $<$<CONFIG:DEBUG>:-fprofile-arcs; -ftest-coverage; --coverage;>)
    #if (BUILD_SHARED_LIBS)
    #    target_compile_options(vga-terminal INTERAFCE 
    #        $<$<CONFIG:DEBUG>:
    #            -O0;-D_GLIBCXX_DEBUG;-D_GLIBCXX_ASSERTIONS;-fexceptions;
    #            -W; -Wshadow; -Wunused-variable;
    #            -Wunused-parameter; -Wunused-function; -Wunused; -Wno-system-headers;
    #            -Wno-deprecated; -Woverloaded-virtual; -Wwrite-strings; -fprofile-arcs; -ftest-coverage;
    #        >
    #        $<$<CONFIG:RELEASE>:-fexceptions;-D_FORTIFY_SOURCE=2>
    #    ) #-Werror)
    #    target_link_options(vga-terminal INTERFACE $<$<CONFIG:DEBUG>:-fprofile-arcs; -ftest-coverage>;)
    #endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang")
    target_compile_options(vga-terminal-static PRIVATE -WCL4  -Wabstract-vbase-init -Wassign-enum
        -Watomic-implicit-seq-cst -Watomic-properties -Wbad-function-cast -Wbinary-literal
        -fPIC 
        $<$<CONFIG:DEBUG>:
            -g;
        >
        $<$<CONFIG:RELEASE>:
            -O2;
        >
    ) #-Weverything #-Werror
    
    
    ### This if Clang is shipped with VS2019 CE
    if (WIN32)
        set(LINK_SUBSYS_WIN /subsystem:windows)
        message(STATUS ${LINK_SUBSYS_WIN})
        target_link_options(vga-terminal-static INTERFACE ${LINK_SUBSYS_WIN})
    elseif(UNIX)
        #target_compile_options(vga-terminal-static PUBLIC -fprofile-instr-generate -fcoverage-mapping)
        target_compile_options(vga-terminal-static INTERFACE 
            $<$<CONFIG:DEBUG>:
                #-flto; -fsanitize=address; -fsanitize=thread; -fsanitize=memory; -fsanitize=undefined; -fsanitize=dataflow;
                #-fsanitize=cfi; -fsanitize=safe-stack;
                #-fprofile-instr-generate; -fcoverage-mapping; 
                -c -ftest-coverage;
            >
            $<$<CONFIG:RELEASE>:
            >
        )
        #target_link_options(vga-terminal-static PUBLIC $<$<CONFIG:DEBUG>: -fprofile-instr-generate; -fcoverage-mapping;>)
        target_link_options(vga-terminal-static INTERFACE $<$<CONFIG:DEBUG>: -ftest-coverage; --coverage;>)
    else()
        message(ERROR "Unknown OS")
    endif()
else()
    message(WARNING ${CMAKE_CXX_COMPILER_ID})
endif()

message("target interface compiler options:")
get_target_property(vga-terminal-static_copts vga-terminal-static COMPILE_OPTIONS)
message(STATUS "vga-terminal-static " ${vga-terminal-static_copts})
unset(vga-terminal-static_copts)
#if (BUILD_SHARED_LIBS)
#    get_target_property(vga-terminal_copts vga-terminal COMPILE_OPTIONS)
#    message(STATUS "vga-terminal " ${vga-terminal_copts})
#    unset(vga-terminal_copts)
#endif()





################################ Examples ####################################
if(BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()

############################### tests ########################################
if(BUILD_TESTING)
add_subdirectory("test")
endif()
