cmake_minimum_required (VERSION 3.16)


find_package(GTest 1.10 CONFIG REQUIRED)

if (TEST_DUMP_SNAPSHOT)
    set (BUILD_SNAPSHOT bool ON) # ON by dependent option configuration anyway, but not hurting
    add_compile_definitions(TEST_DUMP_SNAPSHOT)
else()
    ### VgaTerminal tests ###
    add_executable(runUnitTests "tests.cpp")
    add_test(UnitTestsInMain runUnitTests)
    target_link_libraries(runUnitTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock)
    add_custom_command(TARGET runUnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runUnitTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runUnitTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runUnitTests>
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
    
    add_executable(runSnapshotTests "snapshotTest.cpp")
    target_link_libraries(runSnapshotTests PRIVATE ${LIB_SDL2main} vga-terminal-static GTest::gtest GTest::gmock SDL2::SDL2_image)
    add_test(SnapshotTestsInMain runSnapshotTests)
    add_custom_command(TARGET runSnapshotTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:runSnapshotTests>/snapshot
    )
endif()

### Valgrind tests ###
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_executable(runHelgrindTests "valgrind/helgrind.cpp")
    target_link_libraries(runHelgrindTests PRIVATE ${LIB_SDL2main} vga-terminal-static)
    add_executable(runMultiThreadFuzzTests "valgrind/multithreading.cpp")
    target_link_libraries(runMultiThreadFuzzTests PRIVATE ${LIB_SDL2main} vga-terminal-static)
endif()
