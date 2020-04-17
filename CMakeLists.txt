# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project ("sdl2-vga-terminal" VERSION 0.1 DESCRIPTION "Vga Terminal on SDL2")

option(WITH_EXAMPLES "build examples" ON)
option(WITH_TEST "build test" ON)
option(TEST_DUMP_SNAPSHOT "running test dumping the expected output instead of verifing it" OFF)
option(WITH_SNAPSHOT "running snapshot test" OFF)
option(WITH_SDL2_STATIC "linking STATIC LIB with SDL2 STATIC" OFF)


enable_testing()
# Include sub-projects.
add_subdirectory ("sdl2-vga-terminal")
