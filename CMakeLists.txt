cmake_minimum_required (VERSION 3.8.0)
set(CMAKE_VERBOSE_MAKEFILE on)

project (ShenmueDK LANGUAGES CXX DESCRIPTION "Shenmue I & II HD Modding SDK for C++")

set (SHENDK_VERSION_MAJOR 0)
set (SHENDK_VERSION_MINOR 1)
set (SHENDK_VERSION_BUILD 0)

# c++ stuff
set(CMAKE_CXX_STANDARD 17)
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

# options
option(BUILD_TESTS "Build test programs" ON)

# dependencies

#set(NV_SOURCE_DIR "third_party/nvidia-texture-tools")
#IF(WIN32)
#	# gnuwin32 paths:
#	SET(GNUWIN32_PATH "${NV_SOURCE_DIR}/extern/gnuwin32")
#	SET(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "${GNUWIN32_PATH}/include")
#	SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${GNUWIN32_PATH}/lib")
#
#	# Set GLUT path:
#	SET(GLUT_ROOT_DIR "${NV_SOURCE_DIR}/extern/glut")
#
#	# Set FreeImage path:
#	SET(FREEIMAGE_ROOT_DIR "${NV_SOURCE_DIR}/extern/FreeImage")
#
#ENDIF(WIN32)

# zlib
find_package(zlib REQUIRED)
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    find_library(CXX_FILESYSTEM_SUPPORT NAMES stdc++fs)
endif()
# jsoncpp
find_package(jsoncpp REQUIRED)
# eigen3
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/eigen3)

#set(JSONCPP_WITH_TESTS OFF CACHE BOOL "Compile and (for jsoncpp_check) run JsonCpp test executables" FORCE)
#add_subdirectory(third_party/jsoncpp)
#add_subdirectory(third_party/eigen3)
#include(cmake/NvidiaInclude.cmake)

# sources and stuff
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c src/*.h src/*.hpp)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/third_party/nvidia-texture-tools/src)

# shendk library
add_library(ShenmueDK SHARED ${SOURCES} ${HEADERS})
target_include_directories(ShenmueDK
    PUBLIC include
    PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} jsoncpp)
target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} z)
#target_link_libraries(${PROJECT_NAME) nvcore)
#target_link_libraries(${PROJECT_NAME) nvimage)

# unit tests (gtest)
if (BUILD_TESTS)
  #add_subdirectory(third_party/googletest)
  find_package(GTest)
  include_directories(${GTEST_INCLUDE_DIRS})
  file(GLOB_RECURSE TEST_SOURCES tests/UTest/*.cpp tests/UTest/*.h)
  add_executable(shendk_test ${TEST_SOURCES})
  target_link_libraries(shendk_test PRIVATE ${PROJECT_NAME})
  target_link_libraries(shendk_test PRIVATE ${CXX_FILESYSTEM_SUPPORT})
  target_link_libraries(shendk_test PRIVATE gtest)
  target_link_libraries(shendk_test PRIVATE gmock)
  target_include_directories(shendk_test PRIVATE include PRIVATE src)
endif()
