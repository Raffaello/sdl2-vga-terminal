# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.16)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project ("sdl2-vga-terminal" VERSION 0.2.1 DESCRIPTION "Vga Terminal on SDL2")

################################ Packages ####################################
#set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE}.)
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPACK_SYSTEM_NAME win64)
  else()
    set(CPACK_SYSTEM_NAME win32)
  endif()
endif()
#set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILENAME}-${CMAKE_BUILD_TYPE})
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})

set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
include(CPack)


################################# Options ####################################
option(BUILD_EXAMPLES "build examples" ON)
include(CTest)
option(BUILD_SNAPSHOT "build snapshot test" OFF)
include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(TEST_DUMP_SNAPSHOT "build test to dump the expected output instead of verifing it" OFF
"BUILD_SNAPSHOT" OFF)
option(WITH_SDL2_STATIC "linking STATIC LIB with SDL2 STATIC" OFF)

enable_testing()
add_subdirectory ("sdl2-vga-terminal")
