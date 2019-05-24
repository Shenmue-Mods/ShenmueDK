cmake_minimum_required (VERSION 3.8.0)
set(CMAKE_VERBOSE_MAKEFILE on)

project (ShenmueDK LANGUAGES CXX DESCRIPTION "Shenmue I & II HD Modding SDK for C++")

set(SHENDK_LIB_NAME ShenmueDK)
set(SHENDK_STATIC_LIB_NAME ShenmueDKStatic)
set(SHENDK_VERSION_MAJOR 0)
set(SHENDK_VERSION_MINOR 1)
set(SHENDK_VERSION_BUILD 0)

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
option(BUILD_STATIC "Build shendk static library" ON)
option(BUILD_SHARED "Build shendk shared library" OFF)

# dependencies
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    find_library(CXX_FILESYSTEM_SUPPORT NAMES stdc++fs)
endif()

# jsoncpp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/jsoncpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/jsoncpp/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/extern/jsoncpp/include)

# eigen3
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/eigen3/cmake")
find_package(Eigen3 REQUIRED NO_MODULE)
include_directories(${EIGEN3_INCLUDE_DIR})

# zlib
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/zlib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/zlib)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/extern/zlib)

# tinyxml2
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/tinyxml2)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/tinyxml2)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/extern/tinyxml2)

# sources and stuff
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCES src/*.cpp src/*.c src/*.h src/*.hpp)

# shendk shared library
if (BUILD_SHARED)
    add_library(${SHENDK_LIB_NAME} SHARED ${SOURCES} ${HEADERS})
    target_include_directories(${SHENDK_LIB_NAME}
        PUBLIC include
        PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries (${SHENDK_LIB_NAME} Eigen3::Eigen)
    target_link_libraries(${SHENDK_LIB_NAME} jsoncpp_lib)
    target_link_libraries(${SHENDK_LIB_NAME} zlibstatic)
    target_link_libraries(${SHENDK_LIB_NAME} tinyxml2)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${SHENDK_LIB_NAME} stdc++fs)
    endif()
endif()

# shendk static library
if (BUILD_STATIC)
    add_library(${SHENDK_STATIC_LIB_NAME} STATIC ${SOURCES} ${HEADERS})
    target_include_directories(${SHENDK_STATIC_LIB_NAME}
        PUBLIC include
        PRIVATE src ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries (${SHENDK_STATIC_LIB_NAME} Eigen3::Eigen)
    target_link_libraries(${SHENDK_STATIC_LIB_NAME} jsoncpp_lib)
    target_link_libraries(${SHENDK_STATIC_LIB_NAME} zlibstatic)
    target_link_libraries(${SHENDK_STATIC_LIB_NAME} tinyxml2)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${SHENDK_STATIC_LIB_NAME} stdc++fs)
    endif()
endif()

# unit tests (gtest)
if (BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest)
    include_directories(${GTEST_INCLUDE_DIRS})
    file(GLOB_RECURSE TEST_SOURCES tests/UTest/*.cpp tests/UTest/*.h)
    add_executable(shendk_test ${TEST_SOURCES})
    target_link_libraries(shendk_test PRIVATE ${SHENDK_STATIC_LIB_NAME})
    target_link_libraries(shendk_test PRIVATE ${CXX_FILESYSTEM_SUPPORT})
    target_link_libraries(shendk_test PRIVATE gtest)
    target_link_libraries(shendk_test PRIVATE gmock)
    target_include_directories(shendk_test PRIVATE include)
endif()
