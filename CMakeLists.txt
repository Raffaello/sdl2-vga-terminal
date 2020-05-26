cmake_minimum_required (VERSION 3.16)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project ("sdl2-vga-terminal" VERSION 0.4.0 DESCRIPTION "Vga Terminal on SDL2")


################################ Packages ####################################
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CPACK_SYSTEM_NAME win64)
  else()
    set(CPACK_SYSTEM_NAME win32)
  endif()
endif()
set(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})
set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_GENERATOR "ZIP")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CMAKE_PROJECT_DESCRIPTION})
include(CPack)


################################# Options ####################################
option(BUILD_SHARED_LIBS "build shared libs with C inteface" ON)
option(BUILD_EXAMPLES "build examples" ON)
include(CTest)
option(ENABLE_CODE_COVERAGE "compile Debug with code coverage" OFF)
option(BUILD_SNAPSHOT "build snapshot test" OFF)
include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(TEST_DUMP_SNAPSHOT "build test to dump the expected output instead of verifing it" OFF
"BUILD_SNAPSHOT" OFF)
option(WITH_SDL2_STATIC "linking STATIC LIB with SDL2 STATIC" OFF)
option(HAVE_PRECISE_TIMER "if SDL2 timers are precise" ON)

add_subdirectory ("sdl2-vga-terminal")
