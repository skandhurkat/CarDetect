cmake_minimum_required(VERSION 2.8)

project(CarDetect)

# set modules path
set(CMAKE_MODULE_PATH
    ${CMAKE_ROOT}/Modules
    ${PROJECT_SOURCE_DIR}/cmake
   )

cmake_policy(SET CMP0017 NEW)

include(CheckCXXCompilerFlag)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

if(CMAKE_COMPILER_IS_GNUCXX)
  execute_process(
      COMMAND g++ -dumpversion
      OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION
      )
  if(CMAKE_CXX_COMPILER_VERSION GREATER 4.7 OR
     CMAKE_CXX_COMPILER_VERSION EQUAL 4.7)
  add_definitions("-std=c++11")
  else()
	  message(FATAL_ERROR 
	  "This project requires g++ 4.7 or higher")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g -pg")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O4")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -O4 -g -pg")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  check_cxx_compiler_flag("-stdlib=libc++" CXX0x_SUPPORTED)
  if(CXX0x_SUPPORTED)
    add_definitions("-stdlib=libc++")
  else()
    message(FATAL_ERROR 
	  "Compiler does not support -stdlib=libc++. This project requires C++11")
  endif()
elseif(MSVC)
# Nothing special needed for MSVC
# Just enable all warnings for debug
  set(CMAKE_CXX_FLAGS_DEBUG "/W4")
else()
  message(FATAL_ERROR
  "Your compiler probably does not support C++11. This project requires C++11")
endif()

include_directories(
    ${OpenCV_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    )

link_directories(
    ${OpenCV_LIBRARY_DIRS}
    )

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

subdirs(src data)
