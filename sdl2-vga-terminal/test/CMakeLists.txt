cmake_minimum_required (VERSION 3.16)

find_package(GTest 1.10 CONFIG REQUIRED)

if (TEST_DUMP_SNAPSHOT)
    set (BUILD_SNAPSHOT bool ON) # ON by dependent option configuration anyway, but not hurting
    add_compile_definitions(TEST_DUMP_SNAPSHOT)
else()
    ### Window tests ###
    add_executable(runWindowTests "WindowTest.cpp")
    add_test(WindowTest runWindowTests)
    target_link_libraries(runWindowTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock)
    add_custom_command(TARGET runWindowTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runWindowTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runWindowTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runWindowTests>
    )


    ### VgaTerminal tests ###
    add_executable(runVgaTerminalTests "VgaTerminalTest.cpp")
    add_test(VgaTerminalTest runVgaTerminalTests)
    target_link_libraries(runVgaTerminalTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock)
    add_custom_command(TARGET runVgaTerminalTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runVgaTerminalTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runVgaTerminalTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runVgaTerminalTests>
    )

 

    ### tui::utils Tests ###
    add_executable(runUtilsTests "tui/utilsTest.cpp")
    add_test(UtilsTest runUtilsTests)
    target_link_libraries(runUtilsTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock)
    add_custom_command(TARGET runUtilsTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runUtilsTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runUtilsTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runUtilsTests>
    )
    
    ### Shared Lib tests ###
    if (BUILD_SHARED_LIBS)
        add_executable(runSOTest "so-test.cpp")
        add_test(SOTest runSOTest)
        target_link_libraries(runSOTest PRIVATE ${LIB_SDL2main} vga-terminal GTest::gtest)
        add_custom_command(TARGET runSOTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runSOTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runSOTest>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:vga-terminal> $<TARGET_FILE_DIR:runSOTest>
        )
    endif()
endif()

### Snapshot tests ####
if (BUILD_SNAPSHOT)
    find_package(SDL2-image CONFIG REQUIRED)
    
    ### VgaTerminal Snapshot Tests ###
    add_executable(runVgaTerminalSnapshotTests "VgaTerminalSnapshotTest.cpp")
    target_link_libraries(runVgaTerminalSnapshotTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock SDL2::SDL2_image)
    add_test(VgaTerminalSnapshotTestsInMain runVgaTerminalSnapshotTests)
    add_custom_command(TARGET runVgaTerminalSnapshotTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runVgaTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runVgaTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runVgaTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:runVgaTerminalSnapshotTests>/snapshot
    )

    ### TuiTerminal Snapshot Tests ###
    add_executable(runTuiTerminalSnapshotTests "TuiTerminalSnapshotTest.cpp")
    add_test(TuiTerminalSnapshotTest runTuiTerminalSnapshotTests)
    target_link_libraries(runTuiTerminalSnapshotTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock SDL2::SDL2_image)
    add_custom_command(TARGET runTuiTerminalSnapshotTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runTuiTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runTuiTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runTuiTerminalSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:runTuiTerminalSnapshotTests>/snapshot
    )
endif()

### Valgrind tests ###
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_executable(runHelgrindTests "valgrind/helgrind.cpp")
    target_link_libraries(runHelgrindTests PRIVATE ${LIB_SDL2main} vga-terminal-static)
    add_executable(runMultiThreadFuzzTests "valgrind/multithreading.cpp")
    target_link_libraries(runMultiThreadFuzzTests PRIVATE ${LIB_SDL2main} vga-terminal-static)
endif()
