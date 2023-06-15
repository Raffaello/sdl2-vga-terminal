cmake_minimum_required (VERSION 3.16)

find_package(GTest 1.10 CONFIG REQUIRED)
include(GoogleTest)
include(CMakeParseArguments)

macro(macro_test) # ${ARGN} link libraries
    
    set(prefix TEST)
    set(flags SNAPSHOT DEFAULT SHARELIB)
    set(singleValues EXE FILE)
    set(multiValues LINKS)
    
    cmake_parse_arguments(${prefix} "${flags}" "${singleValues}" "${multiValues}" ${ARGN})
    #message("${EXE} - ${FILE} - ${LINKS}")
    #message("EXE: ${TEST_EXE}")
    #message("FILE: ${TEST_FILE}")
    #message("IS DEFAULT: ${TEST_DEFAULT}")
    #message("IS SNAPSHOT: ${TEST_SNAPSHOT}")
    #message("NEED SHARELIB: ${TEST_SHARELIB}")
    #message("LINKS: ${TEST_LINKS}")
    add_executable (${TEST_EXE} ${TEST_FILE})
    target_link_libraries(${TEST_EXE} PRIVATE ${TEST_LINKS} GTest::gtest GTest::gmock)
    add_custom_command(TARGET ${TEST_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:${TEST_EXE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:${TEST_EXE}>
    )
    
    if(${TEST_SNAPSHOT})
        add_custom_command(
            TARGET ${TEST_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:${TEST_EXE}>/snapshot
        )
    endif()

    if(${TEST_SHARELIB})
        add_custom_command(
            TARGET ${TEST_EXE} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:vga-terminal> $<TARGET_FILE_DIR:${TEST_EXE}>
        )
    endif()
    
    gtest_discover_tests(${TEST_EXE})
endmacro()

if (TEST_DUMP_SNAPSHOT)
    set (BUILD_SNAPSHOT bool ON) # ON by dependent option configuration anyway, but not hurting
    add_compile_definitions(TEST_DUMP_SNAPSHOT)
else()
    ### Window tests ###
    macro_test(
        DEFAULT
        EXE runWindowTests
        FILE "WindowTest.cpp"
        LINKS vga-terminal-static
    )

    ### VgaTerminal tests ###
    macro_test(
        DEFAULT
        EXE runVgaTerminalTests
        FILE "VgaTerminalTest.cpp"
        LINKS vga-terminal-static
    )
 
    ### tui::utils Tests ###
    macro_test(
        DEFAULT
        EXE runUtilsTests
        FILE "tui/utilsTest.cpp"
        LINKS vga-terminal-static
    )
    
    ### Shared Lib tests ###
    if (BUILD_SHARED_LIBS)
        macro_test(
            SHARELIB
            EXE runSOTest
            FILE "so-test.cpp"
            LINKS vga-terminal
        )
    endif()
endif()

### Snapshot tests ####
if (BUILD_SNAPSHOT)
set (CMAKE_DEBUG_POSTFIX d)
    find_package(SDL2_image CONFIG REQUIRED)
    SET(sdl2_image $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
    ### VgaTerminal Snapshot Tests ###
    macro_test(
        SNAPSHOT
        EXE runVgaTerminalSnapshotTests
        FILE "VgaTerminalSnapshotTest.cpp"
        LINKS vga-terminal-static sdl2_image
    )

    ### TuiTerminal Snapshot Tests ###
    macro_test(
        SNAPSHOT
        EXE runTuiTerminalSnapshotTests
        FILE "TuiTerminalSnapshotTest.cpp"
        LINKS vga-terminal-static sdl2_image
    )
endif()

### Valgrind tests ###
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_executable(runHelgrindTests "valgrind/helgrind.cpp")
    target_link_libraries(runHelgrindTests PRIVATE vga-terminal-static)
    add_executable(runMultiThreadFuzzTests "valgrind/multithreading.cpp")
    target_link_libraries(runMultiThreadFuzzTests PRIVATE vga-terminal-static)
endif()
