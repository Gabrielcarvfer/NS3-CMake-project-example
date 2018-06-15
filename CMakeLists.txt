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

#Output executable
set(examples
        first
        second
        third
        fourth
        lte_first
        lte_second
        dyn_package
        poc_scenario_1)

add_executable(NS3-proj-cmake.exe source.cc header.h)
#Link libraries to output executable
target_link_libraries(NS3-proj-cmake.exe "${NS3_LIBRARIES}")

foreach(example ${examples})
    add_executable(${example}.exe examples/${example}.cc)
    target_link_libraries(${example}.exe "${NS3_LIBRARIES}")
endforeach()