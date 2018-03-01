cmake_minimum_required(VERSION 2.8)
project (SLOTTED_UDP)

option(BUILD_TEST "Build Unit tests" OFF)
option(BUILD_APPS "Build apps" ON)

## CHECK FOR COMPILER CAPABILITIES
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
##

add_subdirectory( lib )

if( BUILD_APPS )
    add_subdirectory( apps )
endif()

if( BUILD_TEST )
    add_subdirectory(test)
endif()
