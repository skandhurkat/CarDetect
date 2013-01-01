cmake_minimum_required(VERSION 2.6)

project(CarDetect)

# set modules path
set(CMAKE_MODULE_PATH
    ${CMAKE_ROOT}/Modules
    ${PROJECT_SOURCE_DIR}/cmake
   )

find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    )

link_directories(
    ${OpenCV_LIBRARY_DIRS}
    )

set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} -g -pg -O0)
set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS} -02 -DNDEBUG)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS} -g -pg -O2)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

subdirs(src data)
