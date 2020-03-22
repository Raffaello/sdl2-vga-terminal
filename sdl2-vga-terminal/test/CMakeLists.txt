cmake_minimum_required (VERSION 3.8)
enable_testing()
find_package(GTest CONFIG REQUIRED)
add_executable(runUnitTests "tests.cpp")
#target_link_libraries(runUnitTests PRIVATE vga-terminal GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(runUnitTests PRIVATE vga-terminal GTest::gtest GTest::gmock)
#add_dependencies(runUnitTests vga-terminal)    
add_test(AllTestsInMain runUnitTests)
add_custom_command(TARGET runUnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runUnitTests>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runUnitTests>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runUnitTests>
)