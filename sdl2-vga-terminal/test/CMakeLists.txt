cmake_minimum_required (VERSION 3.8)

find_package(GTest CONFIG REQUIRED)

if (MSVC)
find_package(SDL2-image CONFIG REQUIRED)
elseif(UNIX)
include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
endif()

add_executable(runUnitTests "tests.cpp")

option(TEST_DUMP_SNAPSHOT "running test dumping the expected output instead of verifing it" OFF)

if(TEST_DUMP_SNAPSHOT)
add_compile_definitions(TEST_DUMP_SNAPSHOT)
endif()

#target_link_libraries(runUnitTests PRIVATE vga-terminal GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
if (MSVC)
    target_link_libraries(runUnitTests PRIVATE vga-terminal GTest::gtest SDL2::SDL2_image)
elseif (UNIX)
    target_link_libraries(runUnitTests PRIVATE vga-terminal GTest::gtest GTest::gmock ${SDL2IMAGE_LIBRARIES})
endif()
#add_dependencies(runUnitTests vga-terminal)    
add_test(AllTestsInMain runUnitTests)
add_custom_command(TARGET runUnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest_main> $<TARGET_FILE_DIR:runUnitTests>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gtest> $<TARGET_FILE_DIR:runUnitTests>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:GTest::gmock> $<TARGET_FILE_DIR:runUnitTests>
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/snapshot $<TARGET_FILE_DIR:runUnitTests>/snapshot
)

