cmake_minimum_required (VERSION 3.8.0)

project (ShenmueDK CXX DESCRIPTION "Shenmue I & II HD Modding SDK for C++")

set (SHENDK_VERSION_MAJOR 0)
set (SHENDK_VERSION_MINOR 1)
set (SHENDK_VERSION_BUILD 0)

#include(version.cmake)

set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_DEBUG_POSTFIX d)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	message("GCC")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	message("MSVC")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	set(CMAKE_CXX_FLAGS_DEBUG "/O1")
else()
	message(FATAL_ERROR "No support for this compiler yet!")
endif()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c src/*.h)
file(GLOB_RECURSE HEADERS include/*.h)



# shendk library
add_library(ShenmueDK ${SOURCES} ${HEADERS})
target_include_directories(ShenmueDK
    PUBLIC include
    PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}
)

# unit tests
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
                 "${CMAKE_BINARY_DIR}/googletest-build")

include_directories("${gtest_SOURCE_DIR}/include"
				    "${gmock_SOURCE_DIR}/include")

file(GLOB_RECURSE TEST_SOURCES tests/UTest/*.cpp tests/UTest/*.h)
add_executable(shendk_test ${TEST_SOURCES})
target_link_libraries(shendk_test PRIVATE ${PROJECT_NAME})
target_link_libraries(shendk_test PRIVATE ${CXX_FILESYSTEM_SUPPORT})
target_link_libraries(shendk_test PRIVATE gtest)
target_link_libraries(shendk_test PRIVATE gmock)
target_include_directories(shendk_test PRIVATE include PRIVATE src)
