cmake_minimum_required(VERSION 3.2)

project(lowSpag2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -O3")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
add_subdirectory(dependencies/minifb/ build)
add_subdirectory(src)