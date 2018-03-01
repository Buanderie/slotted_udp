cmake_minimum_required(VERSION 2.8)
project(SUDP_MASTER)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread" )

include_directories(${LIB_SLOTTED_UDP_SOURCE_DIR}/.)

# Add lib
LINK_DIRECTORIES(${LIB_SLOTTED_UDP_BINARY_DIR})
set( DEPS "${DEPS};sudp" )

# Global shit
file (GLOB_RECURSE SEND_SRC ./*.cc)
add_executable(sudp_tx ${SEND_SRC})

target_link_libraries (sudp_tx ${DEPS} )
