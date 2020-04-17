cmake_minimum_required (VERSION 3.8)


find_package(GTest CONFIG REQUIRED)

if (TEST_DUMP_SNAPSHOT)
    set (WITH_SNAPSHOT bool ON)
    add_compile_definitions(TEST_DUMP_SNAPSHOT)
else()
    add_executable(runUnitTests "tests.cpp")
    add_test(UnitTestsInMain runUnitTests)
    target_link_libraries(runUnitTests PRIVATE vga-terminal-obj GTest::gtest GTest::gmock)
    add_custom_command(TARGET runUnitTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runUnitTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runUnitTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runUnitTests>
    )
endif()

if (WITH_SNAPSHOT)
    find_package(SDL2-image CONFIG REQUIRED)

    add_executable(runSnapshotTests "snapshotTest.cpp")
    target_link_libraries(runSnapshotTests PRIVATE vga-terminal-obj GTest::gtest SDL2::SDL2_image)
    add_test(SnapshotTestsInMain runSnapshotTests)
    add_custom_command(TARGET runSnapshotTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runSnapshotTests>
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:runSnapshotTests>/snapshot
    )
endif()

add_executable(runSOTest "so-test.cpp")
add_test(SOTest runSOTest)
target_link_libraries(runSOTest PRIVATE vga-terminal GTest::gtest)
add_custom_command(TARGET runSOTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runSOTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runSOTest>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:vga-terminal> $<TARGET_FILE_DIR:runSOTest>
    )



