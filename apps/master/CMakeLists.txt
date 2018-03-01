cmake_minimum_required(VERSION 2.8)
project(SUDP_MASTER)

include_directories(${LIB_SLOTTED_UDP_SOURCE_DIR}/.)

# Add lib
LINK_DIRECTORIES(${LIB_SLOTTED_UDP_BINARY_DIR})
set( DEPS "${DEPS};sudp" )

# Global shit
file (GLOB_RECURSE MASTER_SRC ./*.cc)
add_executable(sudp_master ${MASTER_SRC})

target_link_libraries (sudp_master ${DEPS} )
