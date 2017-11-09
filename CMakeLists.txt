#######################################
# Required CMake version#
#######################################
cmake_minimum_required( VERSION 3.0 )

#######################################
# Project name                        #
######################################
project(NS3-proj-cmake CXX)
add_definitions(-std=c++11 -g -fPIC)

#Set output folder to ./bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#Import NS3 header, libraries and definitions 
#You can configure version/build type/NS3 location by modifying the NS3.cmake script
#It currently points to NS3_PATH=../NS3, NS3_VER=3.27, NS3_BUILDTYPE=debug
include(NS3.cmake)

#Source and header files
set(source_files source.cpp)
set(header_files header.h)

#Output executable
add_executable(NS3-proj-cmake.exe ${source_files} ${header_files})
add_executable(first.exe examples/first.cpp ${header_files})
add_executable(second.exe examples/second.cpp ${header_files})
add_executable(third.exe examples/third.cpp ${header_files})


#Link libraries to output executable
target_link_libraries(NS3-proj-cmake.exe "${NS3_LIBRARIES}")
target_link_libraries(first.exe "${NS3_LIBRARIES}")
target_link_libraries(second.exe "${NS3_LIBRARIES}")
target_link_libraries(third.exe "${NS3_LIBRARIES}")