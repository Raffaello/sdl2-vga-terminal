cmake_minimum_required (VERSION 3.16)


set (CMAKE_CXX_STANDARD 17)
set (CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 CONFIG REQUIRED)

configure_file(version.h.in version.h)

set(LIB_SDL2 $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
             $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

if(HAVE_PRECISE_TIMER)
else()
    add_compile_definitions(NO_PRECISE_TIMER)
endif()

########################### Static Lib #######################################
set(LIB_NAME vga-terminal-static)
set(LIB_CONF ${LIB_NAME}-config)
set(LIB_VER ${LIB_CONF}-version)
include(CMakePackageConfigHelpers)

add_library(${LIB_NAME} STATIC)
set_target_properties(${LIB_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
install(
    TARGETS ${LIB_NAME}
    EXPORT ${LIB_CONF}
    DESTINATION static/lib
    PUBLIC_HEADER DESTINATION static/include
)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/VgaTerminal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Window.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TuiTerminal.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/exceptions/exceptions.hpp
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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TuiTerminal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tui/utils.cpp
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

######################## add compiler options ################################
if(CMAKE_CXX_COMPILER_ID MATCHES "^MSVC")
# @see https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=vs-2019
    set(CXX_OPTS PRIVATE /W4  #/WX
        $<$<CONFIG:DEBUG>:/Od /GR /GS /Gs /RTC1c /D_ALLOW_RTCc_IN_STL>
        $<$<CONFIG:RELEASE>:/O2 /Ot /GA /GL /Gw>)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^GNU")
    set(CXX_OPTS PRIVATE -Wall -Wextra -pedantic #-fPIC 
        $<$<CONFIG:DEBUG>:
            -O0;-D_GLIBCXX_DEBUG;-D_GLIBCXX_ASSERTIONS;-fexceptions;
            -W; -Wshadow; -Wunused-variable;
            -Wunused-parameter; -Wunused-function; -Wunused; -Wno-system-headers;
            -Wno-deprecated; -Woverloaded-virtual; -Wwrite-strings;
        >
        #$<$<CONFIG:RELEASE>:-O2;-fexceptions;-fstack-clash-protection;-D_FORTIFY_SOURCE=2>
        $<$<CONFIG:RELEASE>:-O2;-fexceptions;-D_FORTIFY_SOURCE=2>)    

    if (ENABLE_CODE_COVERAGE)
        set(CXX_COV_OPTS PRIVATE
            $<$<CONFIG:DEBUG>: -fprofile-arcs; -ftest-coverage;>)
        target_link_options(vga-terminal-static PUBLIC PUBLIC $<$<CONFIG:DEBUG>:-fprofile-arcs; -ftest-coverage; --coverage;>)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang")
    set(CXX_OPTS PRIVATE -WCL4  -Wabstract-vbase-init -Wassign-enum
        -Watomic-implicit-seq-cst -Watomic-properties -Wbad-function-cast -Wbinary-literal
         $<$<CONFIG:DEBUG>: -g;>
        $<$<CONFIG:RELEASE>:-O2;>)

    if(ENABLE_CODE_COVERAGE)
        set(CXX_COV_OPTS PRIVATE
            $<$<CONFIG:DEBUG>:-c;-fprofile-arcs; -ftest-coverage;>)

        #target_link_options(vga-terminal-static PUBLIC $<$<CONFIG:DEBUG>: -fprofile-instr-generate; -fcoverage-mapping;>)
        target_link_options(vga-terminal-static PUBLIC $<$<CONFIG:DEBUG>: -ftest-coverage;-fprofile-arcs; --coverage;>)
    endif()

    if (WIN32)
        set(LINK_SUBSYS_WIN /subsystem:windows)
        message(STATUS ${LINK_SUBSYS_WIN})
        target_link_options(vga-terminal-static PUBLIC ${LINK_SUBSYS_WIN})
    elseif(UNIX)
    else()
        message(ERROR "Unknown OS")
    endif()
else()
    message(WARNING ${CMAKE_CXX_COMPILER_ID})
endif()

message("target compiler options: " ${CXX_OPTS} ${CXX_COV_OPTS})
target_compile_options(vga-terminal-static ${CXX_OPTS} ${CXX_COV_OPTS}) #-Werror)
if (BUILD_SHARED_LIBS)
    target_compile_options(vga-terminal PRIVATE ${CXX_OPTS} ${CXX_COV_OPTS})
endif()

unset(CXX_OPTS)
unset(CXX_COV_OPTS)


################################ Examples ####################################
if(BUILD_EXAMPLES)
    add_subdirectory("examples")
endif()


############################### tests ########################################
if(BUILD_TESTING)
    add_subdirectory("test")
endif()
