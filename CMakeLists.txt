# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project ("sdl2-vga-terminal" VERSION 0.2.1 DESCRIPTION "Vga Terminal on SDL2")

option(BUILD_EXAMPLES "build examples" ON)
include(CTest)
option(BUILD_SNAPSHOT "build snapshot test" OFF)
include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(TEST_DUMP_SNAPSHOT "build test to dump the expected output instead of verifing it" OFF
"BUILD_SNAPSHOT" OFF)
option(WITH_SDL2_STATIC "linking STATIC LIB with SDL2 STATIC" OFF)

enable_testing()
# Include sub-projects.
add_subdirectory ("sdl2-vga-terminal")
