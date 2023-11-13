cmake_minimum_required(VERSION 3.5)
project(pics2bits)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(P2B_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/pics2bits/")

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++14")
#add_compile_options(-O2)
#add_compile_options(-g)

file(GLOB SOURCE_FILES "${SRC_DIR}/Demo.cpp" "${P2B_DIR}/*.cpp")
add_executable(pics2bits ${SOURCE_FILES})

include_directories(${P2B_DIR})
include_directories(${SRC_DIR})

#~~~~~ WATCH OUT FOR CORRECT NAMES OF LIBRARIES LOCATIONS
target_link_libraries(pics2bits ${OpenCV_LIBS})
