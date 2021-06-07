cmake_minimum_required(VERSION 3.1)
project(packer C)

set(CMAKE_C_COMPILER "gcc")

include_directories(${PROJECT_SOURCE_DIR}/utils/jansson/includes)
include_directories(${PROJECT_SOURCE_DIR}/utils/tlv/includes)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

add_executable(packer packer.c packer_encode.c packer_decode.c packer_decode.h)
add_subdirectory(utils)
