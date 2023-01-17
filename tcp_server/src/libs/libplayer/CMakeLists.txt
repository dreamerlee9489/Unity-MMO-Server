cmake_minimum_required(VERSION 3.8)

if (CMAKE_BUILD_TYPE STREQUAL Release) 
	set(MyProjectName player)
else()
	set(CMAKE_BUILD_TYPE Debug)
	set(MyProjectName playerd)
endif()

message(STATUS "CMAKE_BINARY_DIR:${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_BUILD_TYPE:${CMAKE_BUILD_TYPE}")
message(STATUS "ProjectName:${MyProjectName}")

project(${MyProjectName})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "../../../libs")
set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -pthread -DEPOLL")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(../../libs)
include_directories(../../apps/space)

aux_source_directory(. SRCS)
aux_source_directory(./../libserver SRCS)
aux_source_directory(../../apps/space SRCS)

add_library(${MyProjectName} STATIC ${SRCS})
target_link_libraries(${MyProjectName} -lprotobuf)
